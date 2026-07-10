// ============================================================================
//  WellBouy - well-water-quality LoRaWAN sensor node
//  XIAO ESP32-S3 + Wio-SX1262, EU868, OTAA to The Things Network.
//
//  Cycle (one per wake):
//    1. Power up the 5 V analog rail and let the sensors settle.
//    2. Read DS18B20, TDS, turbidity, pH sequentially (>50 ms apart).
//    3. Power the rail down.
//    4. Encode the readings + fixed GPS as Cayenne LPP.
//    5. Join (or resume) the LoRaWAN session and send one unconfirmed uplink.
//    6. Deep-sleep for TX_INTERVAL_S, waking on the RTC timer.
//
//  The LoRaWAN session + nonces are kept in RTC memory across deep sleep so we
//  resume the existing session instead of re-joining every cycle (which would
//  breach the TTN fair-use / join policy).
//
//  RadioLib is pinned to 6.6.0 - see platformio.ini.
// ============================================================================

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <RadioLib.h>
#include <CayenneLPP.h>

#include "config.h"
#include "secrets.h"
#include "sensors/temp_sensor.h"
#include "sensors/tds_sensor.h"
#include "sensors/turbidity_sensor.h"
#include "sensors/ph_sensor.h"

// ----------------------------------------------------------------------------
//  Radio + LoRaWAN node
// ----------------------------------------------------------------------------
//  Module(NSS, DIO1, RST, BUSY) - Wio-SX1262 wiring from config.h.
static SX1262       radio = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY);
static LoRaWANNode  node(&radio, &EU868);

// ----------------------------------------------------------------------------
//  ADS1115 external ADC - shared by TDS (A0), turbidity (A1) and pH (A2).
//  The analog sensors are 5 V; their outputs reach the ADS scaled to <= 3.3 V
//  (TDS direct; turbidity + pH through dividers - see the board / config.h).
// ----------------------------------------------------------------------------
static Adafruit_ADS1115 ads;
static bool             adsReady = false;

static bool adsBegin() {
  const uint8_t addrs[] = { 0x48, 0x49, 0x4A, 0x4B };
  for (uint8_t i = 0; i < 4; i++)
    if (ads.begin(addrs[i])) { DBG("[ADS ] found at 0x%02X\n", addrs[i]); return true; }
  DBG("[ADS ] NOT found (0x48-0x4B) - check I2C wiring\n");
  return false;
}

static float adsMilliVolts(uint8_t ch) {
  float acc = 0;
  for (uint8_t i = 0; i < ADS_SAMPLES; i++)
    acc += ads.computeVolts(ads.readADC_SingleEnded(ch)) * 1000.0f;
  return acc / ADS_SAMPLES;
}

// ----------------------------------------------------------------------------
//  State persisted across deep sleep (survives in RTC slow memory)
// ----------------------------------------------------------------------------
RTC_DATA_ATTR uint16_t bootCount   = 0;
RTC_DATA_ATTR bool     haveSession = false;
RTC_DATA_ATTR uint8_t  rtcNonces [RADIOLIB_LORAWAN_NONCES_BUF_SIZE];
RTC_DATA_ATTR uint8_t  rtcSession[RADIOLIB_LORAWAN_SESSION_BUF_SIZE];
//  Live deep-sleep interval. Seeded from config on cold boot, retained across
//  deep sleep, and adjustable at runtime by a DL_CMD_SET_INTERVAL downlink.
RTC_DATA_ATTR uint32_t txIntervalS = TX_INTERVAL_S;

// ----------------------------------------------------------------------------
//  Helpers
// ----------------------------------------------------------------------------
static void sensorRailOn() {
#if SENSOR_PWR_EN_PIN >= 0
  pinMode(SENSOR_PWR_EN_PIN, OUTPUT);
  digitalWrite(SENSOR_PWR_EN_PIN, SENSOR_PWR_ACTIVE_HIGH ? HIGH : LOW);
#endif
}

static void sensorRailOff() {
#if SENSOR_PWR_EN_PIN >= 0
  digitalWrite(SENSOR_PWR_EN_PIN, SENSOR_PWR_ACTIVE_HIGH ? LOW : HIGH);
#endif
}

static void goToSleep() {
  DBG("[SLEEP] deep sleep for %u s\n", txIntervalS);
  radio.sleep();                 // put the SX1262 into its lowest-power state
  sensorRailOff();
  DBG_FLUSH();
  esp_sleep_enable_timer_wakeup((uint64_t)txIntervalS * 1000000ULL);
  esp_deep_sleep_start();        // never returns
}

// Act on a downlink command received in the RX window after our uplink. This is
// the only moment a Class-A node can hear the server, so commands take effect on
// the NEXT wake, not instantly. Payload: [opcode, args...].
static void handleDownlink(const uint8_t* data, size_t len) {
  if (len == 0) return;
  switch (data[0]) {
    case DL_CMD_SET_INTERVAL:
      if (len >= 2 && data[1] > 0) {
        uint32_t s = (uint32_t)data[1] * 60UL;         // arg is in minutes
        if (s < TX_INTERVAL_MIN_S) s = TX_INTERVAL_MIN_S;
        txIntervalS = s;
        DBG("[DL] set interval -> %u s (%u min)\n", txIntervalS, data[1]);
      }
      break;
    case DL_CMD_REBOOT:
      DBG("[DL] reboot requested\n");
      DBG_FLUSH();
      esp_restart();
      break;
    default:
      DBG("[DL] unknown opcode 0x%02X (%u bytes)\n", data[0], (unsigned)len);
      break;
  }
}

// ----------------------------------------------------------------------------
//  Measurement
// ----------------------------------------------------------------------------
struct Readings {
  float temperatureC;
  float tdsPpm;
  float turbidityNtu;
  float ph;
};

static Readings readAllSensors() {
  sensorRailOn();
  tempSetup();                   // DS18B20 on the 1-Wire bus (not the ADS)

  // Bring up the ADS1115 that carries TDS / turbidity / pH.
  Wire.begin();
  Wire.setClock(ADS_I2C_HZ);
  Wire.setTimeOut(50);
  adsReady = adsBegin();
  if (adsReady) ads.setGain(GAIN_ONE);            // +/-4.096 V full scale

  delay(SENSOR_WARMUP_MS);       // let the 5 V analog front-ends stabilise

  Readings r;
  // Temperature first (compensates the TDS reading). NaN -> assume 25 C.
  r.temperatureC = readTemperature();
  float tc = isnan(r.temperatureC) ? 25.0f : r.temperatureC;

  if (adsReady) {
    delay(SENSOR_GAP_MS);
    r.tdsPpm       = tdsPpmFromMilliVolts(adsMilliVolts(ADS_CH_TDS), tc);
    delay(SENSOR_GAP_MS);
    r.turbidityNtu = turbidityNtuFromMilliVolts(adsMilliVolts(ADS_CH_TURBIDITY));
    delay(SENSOR_GAP_MS);
    r.ph           = phFromMilliVolts(adsMilliVolts(ADS_CH_PH));
  } else {
    r.tdsPpm = 0.0f; r.turbidityNtu = 0.0f; r.ph = 0.0f;
  }

  DBG("[READ] T=%.1f C  TDS=%.0f ppm  Turb=%.0f NTU  pH=%.2f\n",
      r.temperatureC, r.tdsPpm, r.turbidityNtu, r.ph);

  sensorRailOff();
  return r;
}

// ----------------------------------------------------------------------------
//  LoRaWAN
// ----------------------------------------------------------------------------
// Bring the radio up and join/resume the OTAA session. Returns true on success.
static bool loraConnect() {
  DBG("[LORA] radio.begin()\n");
  int16_t state = radio.begin();
  if (state != RADIOLIB_ERR_NONE) {
    DBG("[LORA] radio init failed (%d)\n", state);
    return false;
  }

  uint64_t joinEUI = SECRET_JOIN_EUI;
  uint64_t devEUI  = SECRET_DEV_EUI;
  uint8_t  appKey[] = SECRET_APP_KEY;

  // RadioLib 6.6.0 derives 1.0.x session keys from the *nwkKey* slot and copies
  // both keys unconditionally (a NULL slot crashes). TTN 1.0.x has a single
  // root key, so pass the AppKey into BOTH slots.
  node.beginOTAA(joinEUI, devEUI, appKey, appKey);

  // Restore a previous session from RTC memory if we have one.
  if (haveSession) {
    node.setBufferNonces(rtcNonces);
    node.setBufferSession(rtcSession);
    DBG("[LORA] restoring saved session\n");
  }

  DBG("[LORA] activateOTAA()\n");
  state = node.activateOTAA();
  if (state == RADIOLIB_LORAWAN_NEW_SESSION) {
    DBG("[LORA] joined (new session)\n");
  } else if (state == RADIOLIB_LORAWAN_SESSION_RESTORED) {
    DBG("[LORA] resumed existing session\n");
  } else {
    DBG("[LORA] join failed (%d)\n", state);
    haveSession = false;         // force a fresh join next time
    return false;
  }

  node.setADR(true);             // adaptive data rate ON
  // EU868 has no legal max dwell time (band dwellTimeUp = 0). Ignore any
  // TxParamSetupReq a network sends: otherwise a long-airtime uplink (high SF)
  // is cancelled with ERR_DWELL_TIME_EXCEEDED (-1114) before it ever transmits.
  node.setDwellTime(false);
  return true;
}

// Persist the current nonces + session to RTC so we can resume after sleep.
static void loraSaveSession() {
  uint8_t* nonces  = node.getBufferNonces();
  uint8_t* session = node.getBufferSession();
  if (nonces && session) {
    memcpy(rtcNonces,  nonces,  RADIOLIB_LORAWAN_NONCES_BUF_SIZE);
    memcpy(rtcSession, session, RADIOLIB_LORAWAN_SESSION_BUF_SIZE);
    haveSession = true;
  }
}

// ----------------------------------------------------------------------------
//  Arduino entry points
// ----------------------------------------------------------------------------
void setup() {
  DBG_BEGIN();
  bootCount++;
  DBG("\n==== WellBouy boot #%u ====\n", bootCount);

  // 1-3. Acquire the water-quality readings.
  Readings r = readAllSensors();

  // 4. Encode as Cayenne LPP. Turbidity and TDS use the Luminosity type
  //    (0-65535); Analog Input (0x02) caps at +/-327.67 and both can exceed it.
  //    ch1 temperature (DS18B20), ch2 pH (analog in), ch3 turbidity (NTU, lum),
  //    ch4 TDS (ppm, lum), ch5 fixed GPS. Field names are set by the TTN
  //    formatter (server/ttn-uplink-formatter.js).
  CayenneLPP lpp(51);
  lpp.reset();
  lpp.addTemperature(1, isnan(r.temperatureC) ? 0.0f : r.temperatureC);
  lpp.addAnalogInput(2, r.ph);
  lpp.addLuminosity (3, (uint16_t)(r.turbidityNtu + 0.5f));
  lpp.addLuminosity (4, (uint16_t)(r.tdsPpm + 0.5f));
  // Fixed location: 11 of 27 bytes. Send it only on the first uplink after a
  // power-up and occasionally after, to cut airtime (keeps TTN's map populated).
  if (bootCount == 1 || (bootCount % 24) == 0) {
    lpp.addGPS(5, FIXED_LATITUDE, FIXED_LONGITUDE, FIXED_ALTITUDE);
  }

  // 5. Join/resume and send one unconfirmed uplink (ADR on). Use sendReceive()
  //    - it runs the uplink AND services the RX windows (downlink) in one call.
  //    Plain uplink() leaves the RX windows open, so after deep-sleep resume the
  //    next uplink can return UPLINK_UNAVAILABLE (-1108). NO_DOWNLINK just means
  //    "sent, nothing came back" - normal for unconfirmed traffic.
  if (loraConnect()) {
    DBG("[LORA] uplink %u bytes on port %d\n", lpp.getSize(), LPP_FPORT);
    uint8_t dnData[64];
    size_t  dnLen = sizeof(dnData);
    int16_t state = node.sendReceive(lpp.getBuffer(), lpp.getSize(), LPP_FPORT,
                                     dnData, &dnLen, /*isConfirmed=*/false);
    // RadioLib's downlink() returns ERR_NONE (0) whether or not data arrived -
    // the received payload length (dnLen) is what distinguishes them: >0 = a
    // command came back, 0 = nothing queued.
    if (state == RADIOLIB_ERR_NONE && dnLen > 0) {
      DBG("[LORA] uplink OK, downlink %u bytes\n", (unsigned)dnLen);
      handleDownlink(dnData, dnLen);          // may change txIntervalS / reboot
    } else if (state == RADIOLIB_ERR_NONE || state == RADIOLIB_LORAWAN_NO_DOWNLINK) {
      DBG("[LORA] uplink OK (no downlink)\n");
    } else {
      DBG("[LORA] uplink failed (%d)\n", state);
    }
    loraSaveSession();                        // persist AFTER servicing downlink
  }

  // 6. Deep sleep until the next cycle.
  goToSleep();
}

void loop() {
  // Never reached: setup() always ends in deep sleep, which restarts setup().
}
