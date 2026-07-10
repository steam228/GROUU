// ============================================================================
//  Inject - combined DS18B20 temp + TDS (ADS A0) + turbidity (ADS A1) -> one
//  CayenneLPP uplink.   Build/flash:  pio run -e b_inject -t upload
//
//  Sends every cycle so the pipeline (TTN -> Node-RED -> InfluxDB -> Grafana)
//  gets all three fields:
//    ch1 temperature_1  (DS18B20 water temp, C)
//    ch3 luminosity_3   (turbidity NTU) - luminosity type, not analog (overflow)
//    ch4 luminosity_4   (TDS ppm)
//  If the DS18B20 isn't connected, temperature falls back to 25 C so the field
//  still populates. (pH / ch2 comes with stage 05.)
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

#define TX_INTERVAL_S  30
#define TDS_ADS_CH     0
#define TURB_ADS_CH    1

static Adafruit_ADS1115 ads;
static bool adsReady = false;
static SX1262      radio = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY);
static LoRaWANNode node(&radio, &EU868);
static bool        joined = false;

static bool adsBegin() {
  const uint8_t a[] = { 0x48, 0x49, 0x4A, 0x4B };
  for (uint8_t i = 0; i < 4; i++)
    if (ads.begin(a[i])) { Serial.printf("ADS1115 at 0x%02X\n", a[i]); return true; }
  Serial.println("ADS1115 not found");
  return false;
}

void setup() {
  Serial.begin(115200);
  uint32_t t = millis();
  while (!Serial && (millis() - t) < 2000) {}
  Serial.println("\n=== WellBouy inject: temp + TDS + turbidity -> TTN ===");

  tempSetup();

  Serial.print("radio.begin() ... ");
  int16_t st = radio.begin();
  Serial.println(st == RADIOLIB_ERR_NONE ? "ok" : "FAIL");

  uint64_t je = SECRET_JOIN_EUI;
  uint64_t de = SECRET_DEV_EUI;
  uint8_t  ak[] = SECRET_APP_KEY;
  node.beginOTAA(je, de, ak, ak);
  Serial.print("joining TTN ... ");
  st = node.activateOTAA();
  if (st == RADIOLIB_LORAWAN_NEW_SESSION) {
    Serial.println("joined");
    joined = true;
    node.setADR(true);
    node.setDutyCycle(false);   // BENCH ONLY: ignore EU868 1% duty cycle so 30 s
                                // uplinks all go. Real node uses 600 s (compliant).
  } else {
    Serial.printf("FAIL (%d)\n", st);
  }

  Wire.begin();
  Wire.setClock(100000);
  Wire.setTimeOut(50);
  adsReady = adsBegin();
  if (adsReady) ads.setGain(GAIN_ONE);
}

void loop() {
  float temp = readTemperature();
  if (isnan(temp)) temp = 25.0f;              // fallback if DS18B20 not wired

  float tds = 0.0f, turb = 0.0f;
  if (adsReady) {
    float mv0 = ads.computeVolts(ads.readADC_SingleEnded(TDS_ADS_CH)) * 1000.0f;
    tds = tdsPpmFromMilliVolts(mv0, temp);
    delay(SENSOR_GAP_MS);                     // let the ADS mux settle before A1
    float mv1 = ads.computeVolts(ads.readADC_SingleEnded(TURB_ADS_CH)) * 1000.0f;
    turb = turbidityNtuFromMilliVolts(mv1);
  }
  Serial.printf("temp = %.1f C   tds = %.0f ppm   turb = %.0f NTU\n", temp, tds, turb);

  if (joined) {
    CayenneLPP lpp(20);
    lpp.reset();
    lpp.addTemperature(1, temp);                        // -> temperature_1
    lpp.addLuminosity(3, (uint16_t)(turb + 0.5f));      // -> luminosity_3 (turbidity NTU)
    lpp.addLuminosity(4, (uint16_t)(tds + 0.5f));       // -> luminosity_4 (TDS ppm)
    // sendReceive() does the uplink AND the downlink (closes the RX windows) in
    // one call, so the NEXT uplink is allowed. Plain uplink() without a following
    // downlink() makes the 2nd uplink return UPLINK_UNAVAILABLE (-1108).
    int16_t st = node.sendReceive(lpp.getBuffer(), lpp.getSize(), LPP_FPORT, false);
    // NO_DOWNLINK just means "sent, nothing came back" - normal for unconfirmed.
    bool ok = (st == RADIOLIB_ERR_NONE || st == RADIOLIB_LORAWAN_NO_DOWNLINK);
    Serial.printf("uplink temp+tds+turb: %s (code %d)\n", ok ? "sent" : "FAILED", st);
  }

  delay((uint32_t)TX_INTERVAL_S * 1000UL);
}