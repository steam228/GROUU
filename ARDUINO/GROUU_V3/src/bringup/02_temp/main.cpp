// ============================================================================
//  Bring-up 02 - DS18B20 water temperature (in isolation)
//  Build/flash:  pio run -e b02_temp -t upload && pio device monitor
//
//  Two-step test (per the bring-up plan):
//    SEND_LORA 0  -> serial only: just print clean degrees C (calibration view)
//    SEND_LORA 1  -> also join TTN once and uplink the temperature each cycle
//
//  Wiring: DS18B20 DATA -> D3/GPIO4, 4.7k pull-up DATA->3V3, VCC->3V3, GND->GND.
// ============================================================================

#include <Arduino.h>

#include "config.h"
#include "sensors/temp_sensor.h"

// ---- Flip to 1 once the serial reading looks good, to push it to TTN -------
#define SEND_LORA  1

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

void setup() {
  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0) < 2000) {}
  Serial.println("\n=== WellBouy bring-up 02: DS18B20 temperature ===");
#if SEND_LORA
  Serial.println("mode: SERIAL + LoRaWAN uplink");
#else
  Serial.println("mode: SERIAL only (set SEND_LORA 1 to uplink)");
#endif

  tempSetup();
#if SEND_LORA
  loraJoin();
#endif
}

void loop() {
  float t = readTemperature();

  if (isnan(t)) {
    Serial.println("temperature = --   (DS18B20 not found: check DATA->D3/GPIO4, "
                    "4.7k pullup to 3V3, VCC/GND)");
  } else {
    Serial.printf("temperature = %.2f C\n", t);
  }

#if SEND_LORA
  if (joined && !isnan(t)) {
    CayenneLPP lpp(11);
    lpp.reset();
    lpp.addTemperature(1, t);                          // ch1 -> temperature_1
    int16_t st = node.uplink(lpp.getBuffer(), lpp.getSize(), LPP_FPORT, false);
    Serial.printf("  -> uplink temperature_1: %s\n",
                  st == RADIOLIB_ERR_NONE ? "sent" : "FAILED");
    delay((uint32_t)BRINGUP_TX_INTERVAL_S * 1000UL);
  } else {
    delay(1500);
  }
#else
  delay(1500);
#endif
}
