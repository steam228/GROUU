// ============================================================================
//  Bring-up 05 - pH (Phidgets 1130 pH/ORP adapter) via I2C ADS1115 (channel A2)
//  Build/flash:  pio run -e b05_ph -t upload && pio device monitor
//
//  The 1130 is a 5 V RATIOMETRIC adapter: pH 7 ~ VCC/2 (~2.5 V), swinging UP for
//  acids (pH 4 ~ 3.3-3.6 V) and DOWN for bases. Because it can reach > 3.3 V, its
//  output MUST pass through the 10k/18k divider before the 3.3 V ADS - same
//  reason turbidity needed one. Do NOT wire the raw 1130 output into A2.
//
//  Calibration is TWO-POINT and absorbs the divider automatically (both the cal
//  points and the live reading are measured at the ADC pin):
//    1. Dip the probe in pH 7.0 buffer -> note the "(x.xxx V)" -> PH_CAL_V7
//    2. Dip the probe in pH 4.0 buffer -> note the "(x.xxx V)" -> PH_CAL_V4
//    3. Put both in include/config.h, re-flash, confirm pH reads ~7.0 and ~4.0.
//
//  Two-step: SEND_LORA 0 = serial only; 1 = also uplink pH to TTN (analog_in_2).
// ============================================================================

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

#include "config.h"
#include "sensors/ph_sensor.h"

#define PH_ADS_CHANNEL  2
#define SEND_LORA       1

static Adafruit_ADS1115 ads;
static bool adsReady = false;

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
    uint64_t joinEUI = SECRET_JOIN_EUI;
    uint64_t devEUI  = SECRET_DEV_EUI;
    uint8_t  appKey[] = SECRET_APP_KEY;
    node.beginOTAA(joinEUI, devEUI, appKey, appKey);
    Serial.print("LoRa: joining TTN ... ");
    st = node.activateOTAA();
    if (st != RADIOLIB_LORAWAN_NEW_SESSION) { Serial.printf("join FAIL (%d)\n", st); return; }
    Serial.println("joined");
    node.setADR(true);
    node.setDutyCycle(false);   // BENCH ONLY: ignore EU868 1% cap so 30 s uplinks go
    joined = true;
  }
#endif

static bool adsBegin() {
  const uint8_t addrs[] = { 0x48, 0x49, 0x4A, 0x4B };
  for (uint8_t i = 0; i < 4; i++)
    if (ads.begin(addrs[i])) { Serial.printf("ADS1115 found at 0x%02X\n", addrs[i]); return true; }
  Serial.println("ADS1115 NOT found (0x48-0x4B) - check I2C wiring");
  return false;
}

static float adsMilliVolts(uint8_t ch, uint8_t samples = 8) {
  float acc = 0;
  for (uint8_t i = 0; i < samples; i++)
    acc += ads.computeVolts(ads.readADC_SingleEnded(ch)) * 1000.0f;
  return acc / samples;
}

void setup() {
  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0) < 2000) {}
  Serial.println("\n=== WellBouy bring-up 05: pH via I2C ADS1115 (A2) ===");
  Serial.printf("current cal: pH%.1f@%.3fV, pH%.1f@%.3fV  (edit config.h after dipping)\n",
                PH_CAL_PH7, PH_CAL_V7, PH_CAL_PH4, PH_CAL_V4);
#if SEND_LORA
  Serial.println("mode: SERIAL + LoRaWAN uplink");
  loraJoin();
#else
  Serial.println("mode: SERIAL only (set SEND_LORA 1 to uplink)");
#endif
}

void loop() {
  if (!adsReady) {
    Wire.begin();
    Wire.setClock(100000);
    Wire.setTimeOut(50);
    adsReady = adsBegin();
    if (adsReady) { ads.setGain(GAIN_ONE); Serial.printf("ADS ready - pH on A%d\n", PH_ADS_CHANNEL); }
    delay(1000);
    return;
  }

  for (uint8_t ch = 0; ch < 4; ch++) {
    float mv = ads.computeVolts(ads.readADC_SingleEnded(ch)) * 1000.0f;
    Serial.printf("  A%u = %7.1f mV%s\n", ch, mv, ch == PH_ADS_CHANNEL ? "   <- pH" : "");
  }
  float mv = adsMilliVolts(PH_ADS_CHANNEL);
  float ph = phFromMilliVolts(mv);
  Serial.printf("pH(A%d): %.1f mV (%.3f V) -> pH %.2f\n",
                PH_ADS_CHANNEL, mv, mv / 1000.0f, ph);

#if SEND_LORA
  if (joined) {
    CayenneLPP lpp(11);
    lpp.reset();
    lpp.addAnalogInput(2, ph);            // -> analog_in_2 = pH (fits, 0-14 << 327)
    int16_t st = node.sendReceive(lpp.getBuffer(), lpp.getSize(), LPP_FPORT, false);
    bool ok = (st == RADIOLIB_ERR_NONE || st == RADIOLIB_LORAWAN_NO_DOWNLINK);
    Serial.printf("  -> uplink analog_in_2(pH=%.2f): %s (code %d)\n", ph, ok ? "sent" : "FAILED", st);
    delay((uint32_t)BRINGUP_TX_INTERVAL_S * 1000UL);
  } else { delay(1500); }
#else
  delay(1500);
#endif
}
