// ============================================================================
//  analog_util.h - Shared ADC helper for the analog water-quality sensors.
//
//  Uses analogReadMilliVolts(), which applies the ESP32-S3 factory eFuse ADC
//  calibration, so results are in real millivolts rather than raw counts.
// ============================================================================
#pragma once

#include <Arduino.h>
#include "config.h"

// Averaged reading on `pin`, returned in millivolts.
inline float readAvgMilliVolts(uint8_t pin,
                               uint8_t samples = ADC_SAMPLES,
                               uint8_t gapMs   = ADC_SAMPLE_GAP_MS) {
  uint32_t acc = 0;
  for (uint8_t i = 0; i < samples; i++) {
    acc += analogReadMilliVolts(pin);
    delay(gapMs);
  }
  return (float)acc / (float)samples;
}
