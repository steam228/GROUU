// ============================================================================
//  turbidity_sensor.h - Seeed Grove turbidity (analog, transmittance-type).
//
//  The 5 V sensor output is scaled by a 10k/6.8k divider to stay within the ADC
//  range; the driver recovers the raw sensor voltage and maps it to turbidity
//  using a per-sensor TurbidityCal. The Grove (like the DFRobot SEN0189 it
//  replaced) reads HIGH in clear water and DROPS as the water fouls.
//
//  Absolute NTU needs a formazin standard. Until then a variant reports a
//  CLEAR-ANCHORED RELATIVE INDEX: 0 at clear water, rising to `fullScale` at the
//  fouled reference voltage (linear in recovered sensor volts, monotonic).
// ============================================================================
#pragma once

#include <Arduino.h>

// Per-sensor calibration. One row per turbidity sensor/unit you characterise;
// select the active one with TURBIDITY_CAL in config.h. Keeping this data-driven
// is what lets different sensors (Grove, SEN0189, a second unit's probe) each
// carry their own clear anchor without touching the conversion code.
struct TurbidityCal {
  float       vClear;        // recovered sensor volts in CLEAR water  -> index 0
  float       vFoul;         // recovered sensor volts at full-scale turbid
  float       dividerRatio;  // board divider between sensor output and the ADC pin
  float       fullScale;     // index value reported at (and below) vFoul
  const char* name;          // human label for logs
};

// Known variants (defined in turbidity_sensor.cpp). Add your own alongside.
extern const TurbidityCal TURB_CAL_GROVE;     // Seeed Grove turbidity (current build)
extern const TurbidityCal TURB_CAL_SEN0189;   // legacy DFRobot SEN0189 (reference)

// Prepare the ADC for the turbidity pin. Call once in setup().
void turbiditySetup();

// Convert a millivolt reading taken AT THE ADC/ADS pin (after the divider) into
// turbidity, using an explicit calibration. Source-agnostic, so ADS1115-based
// firmware reuses the same curve by passing an ADS channel reading. Clamped to
// [0, cal.fullScale].
float turbidityFromMilliVolts(float mvAtAdc, const TurbidityCal& cal);

// Same, using the active variant selected by TURBIDITY_CAL in config.h. Named
// *Ntu for call-site compatibility; until a formazin fit lands the value is a
// clear-anchored relative index, not calibrated NTU (see config.h).
float turbidityNtuFromMilliVolts(float mvAtAdc);

// Returns turbidity (clamped >= 0), reading the XIAO ADC pin directly.
float readTurbidity();
