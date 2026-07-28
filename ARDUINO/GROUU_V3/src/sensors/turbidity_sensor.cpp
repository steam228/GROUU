#include "turbidity_sensor.h"

#include "config.h"
#include "analog_util.h"

// --- Sensor variants --------------------------------------------------------
// Grove: characterised on well-bouy01 during bring-up (5 V rail, U5 -> A1):
//   clear water 3.79 V (index 0), heavy milk 0.49 V (index = full scale).
const TurbidityCal TURB_CAL_GROVE = {
  /* vClear       */ 3.79f,
  /* vFoul        */ 0.49f,
  /* dividerRatio */ TURBIDITY_DIVIDER_RATIO,
  /* fullScale    */ TURBIDITY_FULL_SCALE,
  /* name         */ "Seeed Grove",
};

// Legacy DFRobot SEN0189, kept for reference / older units. Its clear-water
// point was ~3.96 V; foul point left at the same divider for a like-for-like
// relative index (swap in a formazin fit if you re-adopt this sensor).
const TurbidityCal TURB_CAL_SEN0189 = {
  /* vClear       */ 3.96f,
  /* vFoul        */ 0.49f,
  /* dividerRatio */ TURBIDITY_DIVIDER_RATIO,
  /* fullScale    */ TURBIDITY_FULL_SCALE,
  /* name         */ "DFRobot SEN0189",
};

void turbiditySetup() {
  analogReadResolution(12);
}

float turbidityFromMilliVolts(float mvAtAdc, const TurbidityCal& cal) {
  // Undo the resistor divider to recover the sensor's true output voltage.
  float vSensor = (mvAtAdc / 1000.0f) / cal.dividerRatio;

  // Clear or clearer -> 0; at/below the fouled reference -> full scale.
  if (vSensor >= cal.vClear) return 0.0f;
  if (vSensor <= cal.vFoul)  return cal.fullScale;

  // Clear-anchored relative index, linear in recovered sensor volts. Monotonic
  // (falling voltage => rising turbidity). Replace with a formazin-fitted curve
  // for true NTU; the anchors above still bound it.
  float frac = (cal.vClear - vSensor) / (cal.vClear - cal.vFoul);
  return frac * cal.fullScale;
}

float turbidityNtuFromMilliVolts(float mvAtAdc) {
  return turbidityFromMilliVolts(mvAtAdc, TURBIDITY_CAL);
}

float readTurbidity() {
  float mvAdc = readAvgMilliVolts(PIN_TURBIDITY);
  float idx   = turbidityNtuFromMilliVolts(mvAdc);
  DBG("[TURB] %.1f mV (adc) -> %.0f idx (%s)\n", mvAdc, idx, TURBIDITY_CAL.name);
  return idx;
}
