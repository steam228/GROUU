// ============================================================================
//  Bring-up 03 - TDS via DFRobot Gravity I2C ADS1115 (in isolation)
//  Build/flash:  pio run -e b03_tds -t upload && pio device monitor
//
//  Signal path: Grove TDS analog out -> ADS1115 input -> I2C -> XIAO
//               (SDA=GPIO5/D4, SCL=GPIO6/D5 on the Grove shield).
//
//  Two-step test (per the bring-up plan):
//    SEND_LORA 0  -> serial only: I2C scan + all 4 ADS channels + TDS ppm
//    SEND_LORA 1  -> also join TTN once and uplink the TDS value each cycle
//
//  No DS18B20 here: temperature compensation uses a 25 C default.
//
//  TDS board: VCC->5V (XIAO 5V pin), GND->GND, analog out into the ADS input
//  set by TDS_ADS_CHANNEL below. Dip the probe in water for a real reading.
// ============================================================================

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

#include "config.h"
#include "sensors/tds_sensor.h"

// ---- Which ADS input the TDS analog wire goes to (A0..A3) ------------------
#define TDS_ADS_CHANNEL   0
// ---- Flip to 1 once the serial reading looks good, to push it to TTN -------
#define SEND_LORA         1

static Adafruit_ADS1115 ads;
static bool adsReady = false;

static const uint8_t PIN_SDA = 5;   // XIAO D4 / GPIO5
static const uint8_t PIN_SCL = 6;   // XIAO D5 / GPIO6
static bool wireStarted = false;

// Read raw idle levels of SDA/SCL with internal pull-ups, WITHOUT using Wire
// (so it can never hang). A healthy idle bus reads HIGH on both. A 0 means that
// line is held low (stuck) - a shorted wire or a dead chip sinking it - which
// is exactly what hangs Wire. Lets us watch the bus recover live while rewiring.
static bool busIdleHigh() {
  // With the ESP32 internal pull-ups: a stuck-low line shows up here.
  pinMode(PIN_SDA, INPUT_PULLUP);
  pinMode(PIN_SCL, INPUT_PULLUP);
  delay(2);
  int sda = digitalRead(PIN_SDA);
  int scl = digitalRead(PIN_SCL);

  // WITHOUT internal pull-ups: if the lines are still high, EXTERNAL pull-ups
  // are present -> the ADS module is powered (its onboard pull-ups are active).
  pinMode(PIN_SDA, INPUT);
  pinMode(PIN_SCL, INPUT);
  delay(2);
  int sdaX = digitalRead(PIN_SDA);
  int sclX = digitalRead(PIN_SCL);

  Serial.printf("bus: intPU[SDA=%d SCL=%d]  extPU[SDA=%d SCL=%d] -> %s\n",
                sda, scl, sdaX, sclX,
                (sdaX && sclX) ? "ext pull-ups PRESENT (ADS powered)"
                               : "NO ext pull-ups (ADS unpowered / not connected)");
  return sda && scl;
}

// ---- Software (bit-banged) I2C probe --------------------------------------
// Toggles SDA/SCL by hand; NEVER uses the ESP32 hardware I2C, so it cannot
// hang. Tells us definitively whether the ADS ACKs its address, independent
// of the Wire driver that's locking up.
static inline void bbDelay()     { delayMicroseconds(6); }
static inline void sdaRelease()  { pinMode(PIN_SDA, INPUT_PULLUP); }
static inline void sdaDriveLow() { pinMode(PIN_SDA, OUTPUT); digitalWrite(PIN_SDA, LOW); }
static inline void sclRelease()  { pinMode(PIN_SCL, INPUT_PULLUP); }
static inline void sclDriveLow() { pinMode(PIN_SCL, OUTPUT); digitalWrite(PIN_SCL, LOW); }

static bool bbClockHigh() {            // release SCL, true if it actually rose
  sclRelease(); bbDelay();
  for (int i = 0; i < 100; i++) { if (digitalRead(PIN_SCL)) return true; delayMicroseconds(2); }
  return false;                        // held low = stuck clock
}
static void bbStart() { sdaRelease(); bbClockHigh(); bbDelay(); sdaDriveLow(); bbDelay(); sclDriveLow(); bbDelay(); }
static void bbStop()  { sdaDriveLow(); bbDelay(); bbClockHigh(); bbDelay(); sdaRelease(); bbDelay(); }

static bool bbWriteByte(uint8_t b) {   // true if ACKed
  for (int i = 0; i < 8; i++) {
    if (b & 0x80) sdaRelease(); else sdaDriveLow();
    bbDelay();
    bbClockHigh(); bbDelay();
    sclDriveLow(); bbDelay();
    b <<= 1;
  }
  sdaRelease(); bbDelay();             // release for ACK bit
  bbClockHigh(); bbDelay();
  int ack = digitalRead(PIN_SDA);      // 0 = ACK
  sclDriveLow(); bbDelay();
  return ack == 0;
}
static bool bbProbe(uint8_t addr7) {
  bbStart();
  bool ack = bbWriteByte((uint8_t)((addr7 << 1) | 0));
  bbStop();
  return ack;
}
static void bitbangScan() {
  if (!bbClockHigh()) { Serial.println("bitbang: SCL stuck LOW - cannot clock the bus"); sclDriveLow(); return; }
  Serial.print("bitbang scan:");
  uint8_t found = 0;
  for (uint8_t a = 0x08; a <= 0x77; a++) {
    if (bbProbe(a)) { Serial.printf(" 0x%02X", a); found++; }
  }
  Serial.println(found ? "" : " (no ACK from any address)");
  sdaRelease(); sclRelease();          // leave bus idle
}

#if SEND_LORA
  #include <RadioLib.h>
  #include <CayenneLPP.h>
  #include "secrets.h"

  #define BRINGUP_TX_INTERVAL_S  30

  static SX1262      radio = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY);
  static LoRaWANNode node(&radio, &EU868);
  static bool        joined = false;

  static void loraJoin() {
    Serial.print("LoRa: radio.begin() ... ");
    int16_t st = radio.begin();
    if (st != RADIOLIB_ERR_NONE) { Serial.printf("FAIL (%d)\n", st); return; }
    Serial.println("ok");

    uint64_t joinEUI  = SECRET_JOIN_EUI;
    uint64_t devEUI   = SECRET_DEV_EUI;
    uint8_t  appKey[] = SECRET_APP_KEY;
    node.beginOTAA(joinEUI, devEUI, appKey, appKey);   // 1.0.x: AppKey in both slots

    Serial.print("LoRa: joining TTN ... ");
    st = node.activateOTAA();
    if (st != RADIOLIB_LORAWAN_NEW_SESSION) { Serial.printf("join FAIL (%d)\n", st); return; }
    Serial.println("joined");
    node.setADR(true);
    joined = true;
  }
#endif

// Print every responding I2C address (confirms the Grove wiring + ADS presence).
static void i2cScan() {
  Serial.println("I2C scan:");
  uint8_t found = 0;
  for (uint8_t a = 1; a < 127; a++) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) { Serial.printf("  device at 0x%02X\n", a); found++; }
  }
  if (!found) Serial.println("  (none - check Grove SDA/SCL/VCC/GND)");
}

// ADS1115 lives at 0x48..0x4B depending on the ADDR pin; try them all.
static bool adsBegin() {
  const uint8_t addrs[] = { 0x48, 0x49, 0x4A, 0x4B };
  for (uint8_t i = 0; i < 4; i++) {
    if (ads.begin(addrs[i])) {
      Serial.printf("ADS1115 found at 0x%02X\n", addrs[i]);
      return true;
    }
  }
  Serial.println("ADS1115 NOT found (0x48-0x4B) - check wiring/address");
  return false;
}

// Averaged millivolts on one ADS channel.
static float adsMilliVolts(uint8_t ch, uint8_t samples = 8) {
  float acc = 0;
  for (uint8_t i = 0; i < samples; i++) {
    acc += ads.computeVolts(ads.readADC_SingleEnded(ch)) * 1000.0f;
  }
  return acc / samples;
}

void setup() {
  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0) < 2000) {}
  Serial.println("\n=== WellBouy bring-up 03: TDS via I2C ADS1115 ===");
#if SEND_LORA
  Serial.println("mode: SERIAL + LoRaWAN uplink");
#else
  Serial.println("mode: SERIAL only (set SEND_LORA 1 to uplink)");
#endif

  // I2C is brought up in loop() only once the bus reads idle-high, so a stuck
  // line reports cleanly instead of hanging Wire here.

#if SEND_LORA
  loraJoin();
#endif
}

void loop() {
  if (!adsReady) {
    if (!busIdleHigh()) {            // bus held low -> diagnose (bit-bang can't hang)
      bitbangScan();
      delay(1500);
      return;
    }
    if (!wireStarted) {
      Wire.begin();
      Wire.setClock(100000);
      Wire.setTimeOut(50);
      wireStarted = true;
    }
    i2cScan();
    adsReady = adsBegin();
    if (adsReady) {
      ads.setGain(GAIN_ONE);        // +/-4.096 V, 0.125 mV/bit
      Serial.printf("ADS ready - TDS on A%d\n", TDS_ADS_CHANNEL);
    }
    delay(1500);
    return;
  }

  // Show all four inputs so it's obvious which one carries the TDS signal
  // (the one that moves when the probe goes into water).
  for (uint8_t ch = 0; ch < 4; ch++) {
    float mv = ads.computeVolts(ads.readADC_SingleEnded(ch)) * 1000.0f;
    Serial.printf("  A%u = %7.1f mV%s\n", ch, mv,
                  ch == TDS_ADS_CHANNEL ? "   <- TDS" : "");
  }

  float mvTds = adsMilliVolts(TDS_ADS_CHANNEL);
  float ppm   = tdsPpmFromMilliVolts(mvTds, 25.0f);   // 25 C default (no DS18B20)
  Serial.printf("TDS(A%d): %.1f mV -> %.1f ppm\n", TDS_ADS_CHANNEL, mvTds, ppm);

#if SEND_LORA
  if (joined) {
    CayenneLPP lpp(11);
    lpp.reset();
    lpp.addLuminosity(4, (uint16_t)(ppm + 0.5f));      // ch4 -> luminosity_4 = ppm
    int16_t st = node.uplink(lpp.getBuffer(), lpp.getSize(), LPP_FPORT, false);
    Serial.printf("  -> uplink luminosity_4(TDS): %s\n",
                  st == RADIOLIB_ERR_NONE ? "sent" : "FAILED");
    delay((uint32_t)BRINGUP_TX_INTERVAL_S * 1000UL);
  } else {
    delay(1500);
  }
#else
  delay(1500);
#endif
}
