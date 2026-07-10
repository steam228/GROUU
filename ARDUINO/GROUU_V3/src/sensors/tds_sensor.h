// ============================================================================
//  tds_sensor.h - Grove / DFRobot Gravity TDS (total dissolved solids).
//
//  Implements DFRobot's Gravity TDS conversion (cubic voltage curve) with
//  temperature compensation to 25 C and a two-point probe constant (kValue,
//  see config.h / README).
// ============================================================================
#pragma once

#include <Arduino.h>

// Prepare the on-chip ADC for the TDS pin (only used by the ESP32-direct path).
void tdsSetup();

// Pure conversion: DFRobot Gravity cubic curve + temperature compensation to
// 25 C, scaled by the two-point kValue. Feed it millivolts from ANY source
// (on-chip ADC or an external ADS1115). This is what the ADS bring-up uses.
float tdsPpmFromMilliVolts(float mv, float temperatureC);

// Convenience: read the on-chip ADC on PIN_TDS and convert. (ESP32-direct path.)
// Returns TDS in ppm (mg/L), compensated to 25 C using `temperatureC`.
float readTDS(float temperatureC);
