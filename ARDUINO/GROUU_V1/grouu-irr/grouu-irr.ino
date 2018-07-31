/**
 * Irrigation Soil Main Probe Firmware for Wifi version of GROUU
 * Based on BHonofre, from Bruno Horta
 * to connect to NODE-RED flow ______
 * 
 * unlicensed with no rights reserved by GROUU 2018 (steam228)
 * 
 * */
//MQTT
#include <PubSubClient.h>
//ESP
#include <ESP8266WiFi.h>'
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
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Timing.h>


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
uint32_t delayMS= 3000;


//configure MQTT server
#define MQTT_BROKER_IP "192.168.1.66"
#define MQTT_BROKER_PORT 1883
#define MQTT_AUTH true
#define MQTT_USERNAME "grouu"
#define MQTT_PASSWORD "herbertolevah"

//free MQTT brokers: https://github.com/mqtt/mqtt.github.io/wiki/public_brokers

#define PAYLOAD_ON "ON"
#define PAYLOAD_OFF "OFF"

//CONSTANTS
const String IDNUMBER = "1"; //change number if adding new
const String HOSTNAME  = "grouu-irr"+IDNUMBER; 
const char * OTA_PASSWORD  = "herbertolevah";
const String MQTT_LOG = "system/log";
const String MQTT_SYSTEM_CONTROL_TOPIC = "system/set/"+HOSTNAME;
//sensors
const String MQTT_WATER_humid_PUBLISH_TOPIC = "grouu-irr/sensor/humid"+IDNUMBER;
const String MQTT_WATER_temp_PUBLISH_TOPIC = "grouu-irr/sensor/temp"+IDNUMBER;
const String MQTT_WATER_moistSur_PUBLISH_TOPIC = "grouu-irr/sensor/moistSur"+IDNUMBER;
const String MQTT_WATER_moistDeep_PUBLISH_TOPIC = "grouu-irr/sensor/moistDeep"+IDNUMBER;
const String MQTT_WATER_soilTemp_PUBLISH_TOPIC = "grouu-irr/sensor/stemp"+IDNUMBER;

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
  wifiManager.resetSettings();
  /*timeout relates to the maximum time before the portal to become innactive*/
  wifiManager.setTimeout(AP_TIMEOUT);
 
  if(!wifiManager.autoConnect(HOSTNAME.c_str())) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
    delay(5000);
  } 
  client.setCallback(callback);

  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;

  pinMode(enableSur,OUTPUT);
  pinMode(enableDeep,OUTPUT);

  digitalWrite(enableSur,LOW);
  digitalWrite(enableDeep,LOW);
  
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
    OTA = true;
    OTABegin = true;
  }else if (payloadStr.equals("REBOOT")){
    ESP.restart();
  }
 }     
} 


//Verifica se a ligação está ativa, caso não este liga-se e subscreve aos tópicos de interesse
bool checkMqttConnection(){
  if (!client.connected()) {
    if (MQTT_AUTH ? client.connect(HOSTNAME.c_str(),MQTT_USERNAME, MQTT_PASSWORD) : client.connect(HOSTNAME.c_str())) {
      //SUBSCRIÇÃO DE TOPICOS
      Serial.println("CONNECTED ON MQTT");
      client.subscribe(MQTT_SYSTEM_CONTROL_TOPIC.c_str());
      //Envia uma mensagem por MQTT para o tópico de log a informar que está ligado
      client.publish(MQTT_LOG.c_str(),(String(HOSTNAME)+" CONNECTED").c_str());
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
//Setup do OTA para permitir updates de Firmware via Wi-Fi
void setupOTA(){
  if (WiFi.status() == WL_CONNECTED && checkMqttConnection()) {
    client.publish(MQTT_LOG.c_str(),(String(HOSTNAME)+" OTA IS SETUP").c_str());
    ArduinoOTA.setHostname(HOSTNAME.c_str());
    ArduinoOTA.setPassword((const char *)OTA_PASSWORD);
    ArduinoOTA.begin();
    client.publish(MQTT_LOG.c_str(),(String(HOSTNAME)+" OTA IS READY").c_str());
  }  
}

void publishValues(){

  // DHT22 values
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    temp = event.temperature;
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" *C");
    String Temp = String(temp,1);
    client.publish(MQTT_WATER_temp_PUBLISH_TOPIC.c_str(), Temp.c_str());
  }

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    humid = event.relative_humidity;
    Serial.print("Humidity: ");
    Serial.print(humid);
    Serial.println("%");
    String Humid = String(humid,1);
    client.publish(MQTT_WATER_humid_PUBLISH_TOPIC.c_str(), Humid.c_str());
  }


  //soilTemperature

    DS18B20.requestTemperatures();
    temp = DS18B20.getTempCByIndex(0);
    String soilTemp = String(temp,1);

    Serial.println(temp);
    client.publish(MQTT_WATER_soilTemp_PUBLISH_TOPIC.c_str(), soilTemp.c_str()); 

  //moisture (surface and deep)

    //read&publishSurface
    digitalWrite(enableSur,HIGH);
    digitalWrite(enableDeep,LOW);    
    float sensorValue = analogRead(moistPort);
    String moistSur = String(sensorValue);
    client.publish(MQTT_WATER_moistSur_PUBLISH_TOPIC.c_str(), moistSur.c_str());

    //read&publishDeep
    digitalWrite(enableSur,LOW);
    digitalWrite(enableDeep,HIGH);    
    sensorValue = analogRead(moistPort);
    String moistDeep = String(sensorValue);
    client.publish(MQTT_WATER_moistDeep_PUBLISH_TOPIC.c_str(), moistDeep.c_str());
    digitalWrite(enableDeep,LOW); 
    

}





