
/**
   Irrigation Soil Main Probe Firmware for Wifi version of GROUU
   Based on BHonofre, from Bruno Horta
   to connect to NODE-RED flow ______

   unlicensed with no rights reserved by GROUU 2018 (steam228)

 * */

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
/////////////////////
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
////////////////////
#include <PubSubClient.h>  //MQTT
////////////////////
#include <WiFiUdp.h> //OTA
#include <ArduinoOTA.h>
//Other Libs
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Timing.h>


#define SERIAL_BAUDRATE 115200


//ID of the Board
const String Instalation = "Habibi"; //Where is it?
const String IDCODE = "1"; //number your probe
const String TYPE = "SoilProbe"; //choose type
const String Host = "Grouu" + Instalation + TYPE + IDCODE; //just change if it is not grouu

//MQTT //MQTT //MQTT
//configure MQTT server
#define MQTT_BROKER_IP "192.168.1.66"
#define MQTT_BROKER_PORT 1883
#define MQTT_AUTH true
#define MQTT_USERNAME "grouu"
#define MQTT_PASSWORD "herbertolevah"

//free MQTT brokers: https://github.com/mqtt/mqtt.github.io/wiki/public_brokers

#define PAYLOAD_ON "ON"
#define PAYLOAD_OFF "OFF"

const String MQTT_LOG = "system/log";
const String MQTT_SYSTEM_CONTROL_TOPIC = "system/set/";
//sensors
const String MQTT_WATER_humid_PUBLISH_TOPIC = Host + "/sensor/humid";
const String MQTT_WATER_temp_PUBLISH_TOPIC = Host + "/sensor/temp";
const String MQTT_WATER_moistSur_PUBLISH_TOPIC = Host + "/sensor/moistSur";
const String MQTT_WATER_moistDeep_PUBLISH_TOPIC = Host + "/sensor/moistDeep";
const String MQTT_WATER_soilTemp_PUBLISH_TOPIC = Host + "/sensor/stemp";

WiFiClient wclient;
PubSubClient client(MQTT_BROKER_IP, MQTT_BROKER_PORT, wclient);

//CONTROL FLAGS
bool OTA = false;
bool OTABegin = false;
bool lastButtonState = false;

//WiFiManager constants
#define AP_TIMEOUT 180

//PORTS and Global Variables

//moist
#define moistPort A0
#define enableSur 5 //GPIO14 to feed surface Moist sensor
#define enableDeep 6 //GPIO12 to feed deep Moist sensor

//soilTemperature

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

//enviromnental - DHT22
float temp;
float humid;
#define DHTPIN            4
#define DHTTYPE           DHT22     // DHT 22 (AM2302)
DHT_Unified dht(DHTPIN, DHTTYPE);

Timing mytimer;
uint32_t delayMS;



void setup() {

  Serial.begin(SERIAL_BAUDRATE);
  //WiFiManager stuff:
  WiFiManager wifiManager;
  //uncomment to reset saved settings
  //wifiManager.resetSettings();
  wifiManager.setTimeout(AP_TIMEOUT);

  if (!wifiManager.autoConnect(Host.c_str())) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
    delay(5000);
  }
  //or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();


  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  client.setCallback(callback);

  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;

  pinMode(enableSur, OUTPUT);
  pinMode(enableDeep, OUTPUT);

  digitalWrite(enableSur, LOW);
  digitalWrite(enableDeep, LOW);

  mytimer.begin(0);
}

///////////////////////////CALLBACK
void callback(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  for (int i=0; i<length; i++) {
    payloadStr += (char)payload[i];
  }
  Serial.println(payloadStr);
  String topicStr = String(topic);
 if(topicStr.equals(MQTT_SYSTEM_CONTROL_TOPIC)){
  if(payloadStr.equals("OTA_ON")){
    OTA = true;
    OTABegin = true;
  }else if (payloadStr.equals("OTA_OFF")){
    OTA = true;
    OTABegin = true;
  }else if (payloadStr.equals("REBOOT")){
    ESP.restart();
  }
 }     
} 

void loop() {
  // put your main code here, to run repeatedly:

}
