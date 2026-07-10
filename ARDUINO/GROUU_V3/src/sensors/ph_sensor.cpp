#include "ph_sensor.h"

#include "config.h"
#include "analog_util.h"

void phSetup() {
  analogReadResolution(12);
}

float phFromMilliVolts(float mv) {
  float voltage = mv / 1000.0f;
  // Two-point linear fit through (V7, pH7) and (V4, pH4).
  float slope = (PH_CAL_PH7 - PH_CAL_PH4) / (PH_CAL_V7 - PH_CAL_V4);
  return slope * (voltage - PH_CAL_V7) + PH_CAL_PH7;
}

float readPH() {
  float mv = readAvgMilliVolts(PIN_PH);
  float ph = phFromMilliVolts(mv);
  DBG("[pH  ] %.3f V -> pH %.2f\n", mv / 1000.0f, ph);
  return ph;
}
