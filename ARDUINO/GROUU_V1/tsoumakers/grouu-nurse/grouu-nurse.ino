/**
 * Nursery Table MVP version Main Probe Firmware for Wifi version of GROUU
 * Based on BHonofre, from Bruno Horta bhonofre.pt
 * to connect to NODE-RED flow ______
 * 
 * unlicensed with no rights reserved by GROUU 2019 (steam228)
 * 
 * */
//MQTT
#include <PubSubClient.h>
//ESP
#include <ESP8266WiFi.h>
//Wi-Fi Manager library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>//https://github.com/tzapu/WiFiManager
//OTA
#include <WiFiUdp.h>
#include <ArduinoOTA.h> 
           
#define AP_TIMEOUT 180
#define SERIAL_BAUDRATE 115200

//Other Libs
//#include <OneWire.h>
//#include <DallasTemperature.h>
#include <Timing.h>


//PORTS and Global Variables

//moist
#define moistPort A0
#define enableSur 1 //GPIO14 to feed surface Moist sensor
#define enableDeep 2 //GPIO12 to feed deep Moist sensor
#define enableSur2 8 //GPIO14 to feed surface Moist sensor
#define enableDeep2 9 //GPIO12 to feed deep Moist sensor
////soilTemperature
//
//#define ONE_WIRE_BUS 9 //GPIO4
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature DS18B20(&oneWire);

//valves

#define p1 6

#define v2 2
#define v3 3
#define v4 4
#define v5 5

#define ls1 7

float stemp;

Timing mytimer;
uint32_t delayMS;


//configure MQTT server
#define MQTT_BROKER_IP "10.116.116.134"
#define MQTT_BROKER_PORT 1883
#define MQTT_AUTH true
#define MQTT_USERNAME "grouu"
#define MQTT_PASSWORD "herbertolevah"

//free MQTT brokers: https://github.com/mqtt/mqtt.github.io/wiki/public_brokers

#define PAYLOAD_ON "ON"
#define PAYLOAD_OFF "OFF"

//CONSTANTS
//ID of the Board
const String Instalation = "Tsoumakers"; //Where is it?
const String IDCODE = "1"; //Number your Device
const String TYPE = "Nursery"; //choose type
const String Host = "Grouu" + Instalation + TYPE + IDCODE; //just change if it is not grouu 
const char * OTA_PASSWORD  = "herbertolevah";
const String MQTT_LOG = Host + "/system/log";
const String MQTT_SYSTEM_CONTROL_TOPIC = Host + "/system/set";

//sensors
const String MS1 = Host + "/sensor/MS1";
const String MS2 = Host + "/sensor/MS2";
const String MS1B = Host + "/sensor/MS1B";
const String MS2B = Host + "/sensor/MS2B";
//const String LS2 = Host + "/sensor/LS1"; //tank Full
const String LS1 = Host + "/sensor/LS2"; //tank Empty
//const String STEMP = Host + "/sensor/stemp"; //soil temperature

//valves & Pumps





WiFiClient wclient;
PubSubClient client(MQTT_BROKER_IP,MQTT_BROKER_PORT,wclient);

//CONTROL FLAGS
bool OTA = false;
bool OTABegin = false;
bool lastButtonState = false;


void setup() {
  
  Serial.begin(SERIAL_BAUDRATE);
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();
  /*timeout relates to the maximum time before the portal to become innactive*/
  wifiManager.setTimeout(AP_TIMEOUT);
 
  if(!wifiManager.autoConnect(Host.c_str())) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
    delay(5000);
  } 
  client.setCallback(callback);


  // moisture pins - might go out on moisture over RF version

  pinMode(enableSur,OUTPUT);
  pinMode(enableDeep,OUTPUT);
  pinMode(enableSur2,OUTPUT);
  pinMode(enableDeep2,OUTPUT);

  digitalWrite(enableSur,LOW);
  digitalWrite(enableDeep,LOW);
  digitalWrite(enableSur2,LOW);
  digitalWrite(enableDeep2,LOW);


  // other output pins pins - valves and pumps

  





  
  mytimer.begin(0);

}


//Chamada de recepção de mensagem 
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
    OTA = false;
    OTABegin = false;
  }else if (payloadStr.equals("REBOOT")){
    ESP.restart();
  }
 }     
} 


//Verifies if there is connection and accepts payloads
bool checkMqttConnection(){
  if (!client.connected()) {
    if (MQTT_AUTH ? client.connect(Host.c_str(),MQTT_USERNAME, MQTT_PASSWORD) : client.connect(Host.c_str())) {
      //SUBSCRIÇÃO DE TOPICOS
      Serial.println("CONNECTED ON MQTT");
      client.subscribe(MQTT_SYSTEM_CONTROL_TOPIC.c_str());
      //Envia uma mensagem por MQTT para o tópico de log a informar que está ligado
      client.publish(MQTT_LOG.c_str(),(String(Host)+" CONNECTED").c_str());
    }
  }
  return client.connected();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (checkMqttConnection()){
      if (mytimer.onTimeout(delayMS)){
          publishValues();
        }
      client.loop();
      if(OTA){
        if(OTABegin){
          setupOTA();
          OTABegin= false;
        }
        ArduinoOTA.handle();
      }
    }
  }
}

//OTA setup
void setupOTA(){
  if (WiFi.status() == WL_CONNECTED && checkMqttConnection()) {
    client.publish(MQTT_LOG.c_str(),(String(Host)+" OTA IS SETUP").c_str());
    ArduinoOTA.setHostname(Host.c_str());
    ArduinoOTA.setPassword((const char *)OTA_PASSWORD);
    ArduinoOTA.begin();
    client.publish(MQTT_LOG.c_str(),(String(Host)+" OTA IS READY").c_str());
  }  
}

void publishValues(){

  //soilTemperature

//    DS18B20.requestTemperatures();
//    stemp = DS18B20.getTempCByIndex(0);
//    String soilTemp = String(stemp,1);
//
//    Serial.println(stemp);
//    client.publish(STEMP.c_str(), soilTemp.c_str()); 

  //moisture (surface and deep)

    //read&publishSurface
    digitalWrite(enableDeep,LOW);   
    digitalWrite(enableSur,HIGH);
    float sensorValue = analogRead(moistPort);
    String moistSur = String(sensorValue);
    Serial.println(moistSur);
    client.publish(MS1.c_str(), moistSur.c_str());

    //read&publishDeep
    digitalWrite(enableSur,LOW);
    digitalWrite(enableDeep,HIGH);    
    sensorValue = analogRead(moistPort);
    String moistDeep = String(sensorValue);
    Serial.println(moistDeep);
    client.publish(MS2.c_str(), moistDeep.c_str());
   

        //read&publishSurface
    digitalWrite(enableDeep2,LOW); 
    digitalWrite(enableSur2,HIGH);
    sensorValue = analogRead(moistPort);
    String moistSur2 = String(sensorValue);
    Serial.println(moistSur2);
    client.publish(MS1.c_str(), moistSur.c_str());

    //read&publishDeep
    digitalWrite(enableSur2,LOW);
    digitalWrite(enableDeep2,HIGH);    
    sensorValue = analogRead(moistPort);
    String moistDeep2 = String(sensorValue);
    Serial.println(moistDeep2);
    client.publish(MS2.c_str(), moistDeep.c_str());
   
    

}
