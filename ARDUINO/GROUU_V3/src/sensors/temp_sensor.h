// ============================================================================
//  temp_sensor.h - DS18B20 water temperature (DallasTemperature + OneWire).
//
//  The temperature reading is also used to compensate the TDS measurement,
//  so read it first in the measurement sequence.
// ============================================================================
#pragma once

#include <Arduino.h>

// Initialise the 1-Wire bus. Call once in setup().
void tempSetup();

// Blocking temperature read in degrees Celsius.
// Returns NAN if the sensor is missing / disconnected.
float readTemperature();
