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

#define DHTPIN D7
#define DHTTYPE DHT22 // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);
// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;

const char *ssid = "freixo";
const char *password = "humbertolevah";

Timing mytimer;
uint32_t delayMS;

// configure MQTT server
#define MQTT_BROKER_IP "192.168.1.226"
#define MQTT_BROKER_PORT 1883
#define MQTT_AUTH false
#define MQTT_USERNAME "grouu"
#define MQTT_PASSWORD "opensourceag"

// CONSTANTS
// ID of the Board
const String Instalation = "Loureiro";                     // Where is it?
const String IDCODE = "1";                                 // number your probe
const String TYPE = "testNode";                            // choose type
const String Host = "Grouu" + Instalation + TYPE + IDCODE; // just change if it is not grouu
const char *OTA_PASSWORD = "norbertolevan";
const String MQTT_LOG = "system/log/" + Host;
const String MQTT_SYSTEM_CONTROL_TOPIC = "system/set/" + Host;
// sensors
const String MQTT_room_sensors_PUBLISH_TOPIC = Host + "/sensors";

WiFiClient wclient;
PubSubClient client(MQTT_BROKER_IP, MQTT_BROKER_PORT, wclient);

// CONTROL FLAGS
bool OTA = false;
bool OTABegin = false;
bool lastButtonState = false;