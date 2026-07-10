#include "temp_sensor.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#include "config.h"

static OneWire           s_oneWire(PIN_ONEWIRE);
static DallasTemperature s_ds(&s_oneWire);

void tempSetup() {
  s_ds.begin();
  s_ds.setResolution(12);          // 12-bit: 0.0625 C, ~750 ms conversion
  s_ds.setWaitForConversion(true); // block until the conversion completes
}

float readTemperature() {
  s_ds.requestTemperatures();
  float t = s_ds.getTempCByIndex(0);
  if (t == DEVICE_DISCONNECTED_C) {
    DBG("[TEMP] DS18B20 not responding\n");
    return NAN;
  }
  DBG("[TEMP] %.2f C\n", t);
  return t;
}
