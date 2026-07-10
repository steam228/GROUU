#include "turbidity_sensor.h"

#include "config.h"
#include "analog_util.h"

void turbiditySetup() {
  analogReadResolution(12);
}

float turbidityNtuFromMilliVolts(float mvAtAdc) {
  // Undo the resistor divider to recover the sensor's true output voltage.
  float vSensor = (mvAtAdc / 1000.0f) / TURBIDITY_DIVIDER_RATIO;

  // At/above the measured clear-water voltage -> 0 NTU.
  if (vSensor >= TURBIDITY_V_CLEAR) return 0.0f;

  // Rescale our clear-water voltage onto the point where DFRobot's raw curve
  // reads 0 NTU (~4.20 V), so clear water = 0 with no discontinuity; then apply
  // the DFRobot SEN0189 quadratic (valid roughly 2.5 V .. 4.2 V).
  float vr  = vSensor * (TURBIDITY_V_DFR0 / TURBIDITY_V_CLEAR);
  float ntu = -1120.4f * vr * vr + 5742.3f * vr - 4352.9f;
  return ntu < 0.0f ? 0.0f : ntu;
}

float readTurbidity() {
  float mvAdc = readAvgMilliVolts(PIN_TURBIDITY);
  float ntu   = turbidityNtuFromMilliVolts(mvAdc);
  DBG("[TURB] %.1f mV (adc) -> %.1f NTU\n", mvAdc, ntu);
  return ntu;
}
