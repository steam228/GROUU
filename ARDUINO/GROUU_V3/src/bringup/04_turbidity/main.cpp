// ============================================================================
//  Bring-up 04 - Turbidity (DFRobot SEN0189) via I2C ADS1115 (channel A1)
//  Build/flash:  pio run -e b04_turbidity -t upload && pio device monitor
//
//  Signal path: SEN0189 analog out -> ADS1115 A1 -> I2C -> XIAO.
//  Conversion: DFRobot quadratic voltage->NTU. Clear water reads ~0 NTU.
//
//  TURB_DIVIDER: 1.0 if the sensor output goes straight to the ADS (dev setup,
//  ADS powered at 5 V). Set 0.4048 if you fit the 10k/6.8k divider (ADS at 3.3 V).
//
//  Two-step (per the plan): SEND_LORA 0 = serial only; 1 = also uplink to TTN.
// ============================================================================

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

#include "config.h"

#define TURB_ADS_CHANNEL  1
#define TURB_DIVIDER      0.4048f // 10k/6.8k divider: A1 = sensor voltage x 0.405
// Clear-water calibration for THIS rig (5 V supply, 10k/6.8k divider). Measured
// clear-water sensor voltage = ~3.97 V (A1 ~1606 mV). We anchor 0 NTU here and
// rescale the DFRobot curve onto it (see turbidityNtu), so clear water = 0 NTU
// with no jump. Re-measure and update if the supply/sensor changes.
#define TURB_V_CLEAR      3.96f   // measured sensor volts in clear water -> 0 NTU
#define TURB_V_DFR0       4.20f   // voltage at which the raw DFRobot curve = 0 NTU
#define SEND_LORA         1

static const uint8_t PIN_SDA = 5;   // XIAO D4 / GPIO5
static const uint8_t PIN_SCL = 6;   // XIAO D5 / GPIO6
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
    node.setDutyCycle(false);   // BENCH ONLY: ignore EU868 1% cap so 30 s uplinks
                                // all go. Real node uses 600 s (compliant).
    joined = true;
  }
#endif

static void i2cScan() {
  Serial.println("I2C scan:");
  uint8_t found = 0;
  for (uint8_t a = 1; a < 127; a++) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) { Serial.printf("  device at 0x%02X\n", a); found++; }
  }
  if (!found) Serial.println("  (none - check Grove SDA/SCL/VCC/GND)");
}

static bool adsBegin() {
  const uint8_t addrs[] = { 0x48, 0x49, 0x4A, 0x4B };
  for (uint8_t i = 0; i < 4; i++)
    if (ads.begin(addrs[i])) { Serial.printf("ADS1115 found at 0x%02X\n", addrs[i]); return true; }
  Serial.println("ADS1115 NOT found (0x48-0x4B) - check wiring/address");
  return false;
}

static float adsMilliVolts(uint8_t ch, uint8_t samples = 8) {
  float acc = 0;
  for (uint8_t i = 0; i < samples; i++)
    acc += ads.computeVolts(ads.readADC_SingleEnded(ch)) * 1000.0f;
  return acc / samples;
}

// SEN0189 voltage->NTU. The DFRobot quadratic reads 0 NTU at ~4.20 V, but this
// rig's clear water tops out at TURB_V_CLEAR (~3.97 V). We map clear-water volts
// onto the curve's 0 point so clear = 0 NTU and dirtier water (lower volts) rises
// smoothly. Absolute NTU is approximate (no formazin standard); clear = 0 is the
// anchored reference, and the sign/trend is what matters for the well.
static float turbidityNtu(float mv) {
  float v = (mv / 1000.0f) / TURB_DIVIDER;           // recover sensor-side voltage
  if (v >= TURB_V_CLEAR) return 0.0f;                // clear or clearer -> 0 NTU
  float vr = v * (TURB_V_DFR0 / TURB_V_CLEAR);       // anchor clear -> curve's 0 point
  float ntu = -1120.4f * vr * vr + 5742.3f * vr - 4352.9f;
  return ntu < 0.0f ? 0.0f : ntu;
}

void setup() {
  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0) < 2000) {}
  Serial.println("\n=== WellBouy bring-up 04: Turbidity via I2C ADS1115 (A1) ===");
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
    i2cScan();
    adsReady = adsBegin();
    if (adsReady) { ads.setGain(GAIN_ONE); Serial.printf("ADS ready - turbidity on A%d\n", TURB_ADS_CHANNEL); }
    delay(1000);
    return;
  }

  for (uint8_t ch = 0; ch < 4; ch++) {
    float mv = ads.computeVolts(ads.readADC_SingleEnded(ch)) * 1000.0f;
    Serial.printf("  A%u = %7.1f mV%s\n", ch, mv, ch == TURB_ADS_CHANNEL ? "   <- TURB" : "");
  }
  float mv  = adsMilliVolts(TURB_ADS_CHANNEL);
  float vs  = (mv / 1000.0f) / TURB_DIVIDER;   // recovered sensor voltage
  float ntu = turbidityNtu(mv);
  Serial.printf("Turbidity(A%d): %.1f mV (sensor %.2f V) -> %.1f NTU\n",
                TURB_ADS_CHANNEL, mv, vs, ntu);

#if SEND_LORA
  if (joined) {
    CayenneLPP lpp(11);
    lpp.reset();
    // Luminosity (0x65, 0-65535), NOT AnalogInput: NTU can exceed the 327.67
    // ceiling of AnalogInput. -> luminosity_3, renamed to "turbidity" by the
    // TTN formatter. sendReceive() closes the RX window so the next uplink goes.
    lpp.addLuminosity(3, (uint16_t)(ntu + 0.5f));
    int16_t st = node.sendReceive(lpp.getBuffer(), lpp.getSize(), LPP_FPORT, false);
    bool ok = (st == RADIOLIB_ERR_NONE || st == RADIOLIB_LORAWAN_NO_DOWNLINK);
    Serial.printf("  -> uplink luminosity_3(NTU=%.0f): %s (code %d)\n", ntu, ok ? "sent" : "FAILED", st);
    delay((uint32_t)BRINGUP_TX_INTERVAL_S * 1000UL);
  } else { delay(1500); }
#else
  delay(1500);
#endif
}