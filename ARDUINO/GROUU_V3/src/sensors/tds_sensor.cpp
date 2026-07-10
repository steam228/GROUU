#include "tds_sensor.h"

#include "config.h"
#include "analog_util.h"

void tdsSetup() {
  analogReadResolution(12);   // 0..4095, paired with analogReadMilliVolts()
}

float tdsPpmFromMilliVolts(float mv, float temperatureC) {
  float voltage = mv / 1000.0f;

  // Temperature compensation: normalise the reading back to 25 C.
  // Standard coefficient for aqueous TDS is ~2 %/C.
  float comp  = 1.0f + 0.02f * (temperatureC - 25.0f);
  float vComp = voltage / comp;

  // DFRobot Gravity TDS cubic curve, scaled by the two-point kValue.
  float tds = (133.42f * vComp * vComp * vComp
               - 255.86f * vComp * vComp
               + 857.39f * vComp) * 0.5f * TDS_KVALUE;

  if (tds < 0.0f) tds = 0.0f;
  return tds;
}

float readTDS(float temperatureC) {
  float mv  = readAvgMilliVolts(PIN_TDS);
  float tds = tdsPpmFromMilliVolts(mv, temperatureC);
  DBG("[TDS ] %.1f mV -> %.1f ppm  (k=%.3f)\n", mv, tds, TDS_KVALUE);
  return tds;
}
