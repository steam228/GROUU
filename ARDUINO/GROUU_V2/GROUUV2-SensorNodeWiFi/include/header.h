#include <Arduino.h>
#include <WiFi.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

// MQTT
#include <PubSubClient.h>

#define AP_TIMEOUT 180
#define SERIAL_BAUDRATE 115200

// OTA
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <Timing.h>