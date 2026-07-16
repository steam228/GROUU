#include "ph_sensor.h"

#include "config.h"
#include "analog_util.h"

void phSetup() {
  analogReadResolution(12);
}

float phFromMilliVolts(float mv) {
  float voltage = mv / 1000.0f;
  // Three-point segmented fit, pivoting at pH 7 (the electrode's zero point).
  // Acidic (V <= V7) uses the pH4..pH7 slope; basic (V > V7) uses pH7..pH10.
  // Continuous at pH 7 and exact at all three buffers.
  float slope = (voltage <= PH_CAL_V7)
              ? (PH_CAL_PH7  - PH_CAL_PH4) / (PH_CAL_V7  - PH_CAL_V4)
              : (PH_CAL_PH10 - PH_CAL_PH7) / (PH_CAL_V10 - PH_CAL_V7);
  return slope * (voltage - PH_CAL_V7) + PH_CAL_PH7;
}

float readPH() {
  float mv = readAvgMilliVolts(PIN_PH);
  float ph = phFromMilliVolts(mv);
  DBG("[pH  ] %.3f V -> pH %.2f\n", mv / 1000.0f, ph);
  return ph;
}
