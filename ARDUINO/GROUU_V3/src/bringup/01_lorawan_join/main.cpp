// ============================================================================
//  Bring-up 01 - LoRaWAN comms only
//  Build/flash:  pio run -e b01_lorawan -t upload && pio device monitor
//
//  Goal: prove the radio link end to end BEFORE any sensor exists.
//    - radio.begin()    -> SX1262 wiring (NSS/DIO1/RST/BUSY + SPI) is correct
//    - activateOTAA()    -> keys in secrets.h are right, gateway is up, TTN joins
//    - uplink()          -> a dummy payload shows up on the TTN "Live data" tab
//
//  This sketch does NOT deep-sleep: it joins once in setup() and then sends an
//  incrementing counter every BRINGUP_TX_INTERVAL_S so you can watch it live.
//
//  TTN: set the uplink payload formatter to "CayenneLPP". You should see
//       digital_in_1 counting up (0,1,2,...) and analog_in_2 = 3.14.
// ============================================================================

#include <Arduino.h>
#include <RadioLib.h>
#include <CayenneLPP.h>

#include "config.h"     // LoRa pins, LPP_FPORT
#include "secrets.h"    // OTAA keys

// Faster cadence than production so bring-up is quick to observe.
// (Stay within TTN fair use - don't leave this running for hours.)
#define BRINGUP_TX_INTERVAL_S   30

static SX1262      radio = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY);
static LoRaWANNode node(&radio, &EU868);

static uint32_t counter = 0;

// Print an error and stop - bring-up failures should be loud and obvious.
static void halt(const char* what, int16_t state) {
  Serial.printf("  FAILED: %s (code %d)\n", what, state);
  Serial.println("  halting - fix the above and reset the board.");
  while (true) { delay(1000); }
}

void setup() {
  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0) < 2000) {}
  Serial.println("\n=== WellBouy bring-up 01: LoRaWAN join ===");

  Serial.print("[1/3] radio.begin() ........ ");
  int16_t state = radio.begin();
  if (state != RADIOLIB_ERR_NONE) halt("radio init (check SX1262 wiring/SPI)", state);
  Serial.println("ok");

  uint64_t joinEUI  = SECRET_JOIN_EUI;
  uint64_t devEUI   = SECRET_DEV_EUI;
  uint8_t  appKey[] = SECRET_APP_KEY;
  // RadioLib 6.6.0 derives 1.0.x session keys from the *nwkKey* slot and copies
  // both keys unconditionally (a NULL slot crashes). TTN 1.0.x has a single
  // root key, so pass the AppKey into BOTH slots.
  node.beginOTAA(joinEUI, devEUI, appKey, appKey);

  Serial.print("[2/3] OTAA join to TTN ..... ");
  state = node.activateOTAA();
  if (state != RADIOLIB_LORAWAN_NEW_SESSION) {
    halt("OTAA join (check keys, gateway online, EU868 coverage)", state);
  }
  Serial.println("joined!");

  node.setADR(true);
  Serial.printf("[3/3] sending a dummy uplink every %d s on port %d ...\n",
                BRINGUP_TX_INTERVAL_S, LPP_FPORT);
}

void loop() {
  CayenneLPP lpp(11);
  lpp.reset();
  lpp.addDigitalInput(1, counter & 0xFF);   // counts 0..255 then wraps
  lpp.addAnalogInput(2, 3.14f);             // fixed marker value

  Serial.printf("uplink #%lu (%u bytes) ... ", (unsigned long)counter, lpp.getSize());
  int16_t state = node.uplink(lpp.getBuffer(), lpp.getSize(), LPP_FPORT, /*confirmed=*/false);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("sent");
  } else {
    Serial.printf("uplink failed (code %d)\n", state);
  }

  counter++;
  delay((uint32_t)BRINGUP_TX_INTERVAL_S * 1000UL);
}
