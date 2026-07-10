// ============================================================================
//  turbidity_sensor.h - DFRobot SEN0189 turbidity (analog).
//
//  The 5 V sensor output is scaled down by a 10k / 6.8k divider to stay within
//  the ESP32-S3 ADC range; we recover the raw voltage and apply DFRobot's
//  quadratic voltage->NTU curve. Divider ratio is in config.h.
// ============================================================================
#pragma once

#include <Arduino.h>

// Prepare the ADC for the turbidity pin. Call once in setup().
void turbiditySetup();

// Convert a millivolt reading taken AT THE ADC (after the 10k/6.8k divider) into
// NTU. Source-agnostic, so ADS1115-based firmware can reuse the calibrated curve
// by passing an ADS channel reading. Clamped to >= 0.
float turbidityNtuFromMilliVolts(float mvAtAdc);

// Returns turbidity in NTU (clamped to >= 0), reading the XIAO ADC pin directly.
float readTurbidity();
