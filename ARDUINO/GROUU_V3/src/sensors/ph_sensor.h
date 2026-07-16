// ============================================================================
//  ph_sensor.h - Phidgets 1130 pH adapter (analog).
//
//  Three-point calibration (pH 4.0 / 7.0 / 10.0), segmented at pH 7 to handle
//  the electrode's differing acid/base slopes; calibration voltages in config.h.
// ============================================================================
#pragma once

#include <Arduino.h>

// Prepare the ADC for the pH pin. Call once in setup().
void phSetup();

// Convert a millivolt reading taken AT THE ADC into pH via the segmented fit.
// Source-agnostic, so ADS1115-based firmware can reuse the calibration.
float phFromMilliVolts(float mv);

// Returns pH (unitless, ~0..14), reading the XIAO ADC pin directly.
float readPH();
