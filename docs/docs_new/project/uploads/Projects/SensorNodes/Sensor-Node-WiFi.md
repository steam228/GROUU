---
tags: [grouu, arduino, esp32, mqtt, wifi, dht22, ota, code]
status: done
location: Loureiro
github: "https://github.com/steam228/GROUU/tree/master/ARDUINO/GROUU_V2/GROUUV2-SensorNodeWiFi"
---

# Sensor Node — WiFi Example

← [[../MOC-Projects|🛠️ Projects]] · [[DOCS-HomeServer|Server Stack]] · [[../../HOME|🌿 HOME]]

**GitHub:** [`GROUU/ARDUINO/GROUU_V2/GROUUV2-SensorNodeWiFi`](https://github.com/steam228/GROUU/tree/master/ARDUINO/GROUU_V2/GROUUV2-SensorNodeWiFi)

---

## Status ✅

- [x] Basic code written
- [x] PlatformIO version done
- [x] Documented + pushed to git (saved on new Mac — Arduino sketchbook)

---

## Configuration

| Parameter | Value |
|---|---|
| Installation | `Loureiro` |
| Node ID | `1` |
| Type | `testNode` |
| Host name | `GrouuLoureirotestNode1` |
| MQTT Broker | `grouu01.local:1883` |
| WiFi SSID | `freixo` |
| Sensor | DHT22 (pin D7) |
| Publish topic | `GrouuLoureirotestNode1/sensors` |
| Log topic | `system/log/GrouuLoureirotestNode1` |
| Control topic | `system/set/GrouuLoureirotestNode1` |

---

## Features

- WiFi connection (with reconnect logic)
- MQTT publish/subscribe (PubSubClient)
- DHT22 temperature + humidity readings every 3s
- JSON payload (`{"t2": x.x, "h2": x.x}`)
- **OTA (Over-The-Air updates)** — activate via MQTT control topic (`OTA_ON`)
- Remote reboot via MQTT (`REBOOT`)

---

## Full Code

```cpp
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Timing.h>

#define DHTPIN  D7
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
float t = 0.0;
float h = 0.0;

const char* ssid     = "freixo";
const char* password = "humbertolevah";

Timing mytimer;
uint32_t delayMS;

#define MQTT_BROKER_IP   "grouu01.local"
#define MQTT_BROKER_PORT 1883
#define MQTT_AUTH        false
#define MQTT_USERNAME    "grouu"
#define MQTT_PASSWORD    "opensourceag"

const String Instalation = "Loureiro";
const String IDCODE      = "1";
const String TYPE        = "testNode";
const String Host        = "Grouu" + Instalation + TYPE + IDCODE;
const char * OTA_PASSWORD = "norbertolevan";

const String MQTT_LOG                   = "system/log/" + Host;
const String MQTT_SYSTEM_CONTROL_TOPIC  = "system/set/" + Host;
const String MQTT_room_sensors_PUBLISH_TOPIC = Host + "/sensors";

WiFiClient wclient;
PubSubClient client(MQTT_BROKER_IP, MQTT_BROKER_PORT, wclient);

bool OTA = false;
bool OTABegin = false;

void setup() {
  Serial.begin(115200);
  dht.begin();
  delayMS = 3000;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi connected — IP: " + WiFi.localIP().toString());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  for (int i = 0; i < length; i++) payloadStr += (char)payload[i];
  String topicStr = String(topic);
  if (topicStr.equals(MQTT_SYSTEM_CONTROL_TOPIC)) {
    if      (payloadStr.equals("OTA_ON"))  { OTA = true; OTABegin = true; }
    else if (payloadStr.equals("REBOOT"))  { ESP.restart(); }
  }
}

bool checkMqttConnection() {
  if (!client.connected()) {
    if (client.connect(Host.c_str())) {
      client.subscribe(MQTT_SYSTEM_CONTROL_TOPIC.c_str());
      client.publish(MQTT_LOG.c_str(), (Host + " CONNECTED").c_str());
    }
  }
  return client.connected();
}

void publishValues() {
  StaticJsonDocument<80> doc;
  char output[80];
  float newT = dht.readTemperature();
  float newH = dht.readHumidity();
  if (!isnan(newT)) {
    doc["t2"] = newT;
    doc["h2"] = newH;
    serializeJson(doc, output);
    client.publish(MQTT_room_sensors_PUBLISH_TOPIC.c_str(), output);
    Serial.println(output);
  }
}

void setupOTA() {
  ArduinoOTA.setHostname(Host.c_str());
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.begin();
  client.publish(MQTT_LOG.c_str(), (Host + " OTA IS READY").c_str());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED && checkMqttConnection()) {
    if (mytimer.onTimeout(delayMS)) publishValues();
    client.loop();
    if (OTA) {
      if (OTABegin) { setupOTA(); OTABegin = false; }
      ArduinoOTA.handle();
    }
  }
}
```

---

## MQTT Topic Structure

```
GrouuLoureirotestNode1/sensors         ← publishes {"t2":x,"h2":x}
system/set/GrouuLoureirotestNode1      ← subscribes (OTA_ON / REBOOT)
system/log/GrouuLoureirotestNode1      ← publishes status logs
```

---

## Libraries Required

- `WiFi.h` (ESP32 built-in)
- `Adafruit_Sensor` + `DHT`
- `ArduinoJson`
- `PubSubClient`
- `ArduinoOTA`
- `Timing` (custom or library)

---

## 🔗 Related

- [[DOCS-HomeServer|Server Stack]] — where this data lands (Mosquitto → InfluxDB → Grafana)
- [[../GROUU-Setup-Loureiro|Loureiro Setup]] — deployment location
- [[../SensorNodes/LoRaWAN|GROUUWAN]] — parallel LoRaWAN track for remote/outdoor sensing
- [[../GROUU-Architecture|GROUU Architecture]] — how WiFi and LoRaWAN tracks fit together
- [[../../Refs|Refs]] — WiFi/MQTT reference links
- [GitHub: GROUUV2-SensorNodeWiFi](https://github.com/steam228/GROUU/tree/master/ARDUINO/GROUU_V2/GROUUV2-SensorNodeWiFi)

---

## 🏷️ Tags

`#arduino` `#esp32` `#mqtt` `#wifi` `#dht22` `#ota` `#grouu` `#code` `#loureiro`
