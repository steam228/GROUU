


// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.
// Depends on the following Arduino libraries:
// - Adafruit Unified Sensor Library: https://github.com/adafruit/Adafruit_Sensor
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library



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
//Other Libs
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Timing.h>


#define AP_TIMEOUT 180
#define SERIAL_BAUDRATE 115200

//MQTT
#define MQTT_AUTH false
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""

//PORTS
#define RELAY_ONE 5
#define RELAY_TWO 4
#define TOUCH 13

//pH
#define pHsensor A0

//DHT
#define DHTPIN            2        
#define DHTTYPE           DHT22     // DHT 22 (AM2302)
DHT_Unified dht(DHTPIN, DHTTYPE);

//LEDS
#define red 13
#define yellow 14
#define green 2

#define lumPin A0

Timing mytimer;
uint32_t delayMS;

//CONSTANTS
const String HOSTNAME  = "grouu";
const char * OTA_PASSWORD  = "herbertolevah";
const String MQTT_LOG = "system/log";
const String MQTT_SYSTEM_CONTROL_TOPIC = "system/set/"+HOSTNAME;
//sensors
const String MQTT_TEMP_PUBLISH_TOPIC = "sensor/sala/temp";
const String MQTT_HUMID_PUBLISH_TOPIC = "sensor/sala/humid";
const String MQTT_LIGHT_PUBLISH_TOPIC = "sensor/sala/light";

//MQTT BROKERS GRATUITOS PARA TESTES https://github.com/mqtt/mqtt.github.io/wiki/public_brokers
const char* MQTT_SERVER = "192.168.1.66";

WiFiClient wclient;
PubSubClient client(MQTT_SERVER,1883,wclient);

//CONTROL FLAGS
bool OTA = false;
bool OTABegin = false;
bool lastButtonState = false;

//otherVariables

float temp;
float humid;
float light;



void setup() {

  Serial.begin(SERIAL_BAUDRATE);
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();
  /*define o tempo limite até o portal de configuração ficar novamente inátivo,
  útil para quando alteramos a password do AP*/
  wifiManager.setTimeout(AP_TIMEOUT);
  if(!wifiManager.autoConnect(HOSTNAME.c_str())) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
    delay(5000);
  }
  client.setCallback(callback);
  pinMode(red,OUTPUT);
  pinMode(yellow,OUTPUT);
  pinMode(green,OUTPUT);

  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;

  digitalWrite(red,LOW);
  digitalWrite(yellow,LOW);
  digitalWrite(green,LOW);
  mytimer.begin(0);
}

void publica(){
  sensors_event_t event;
  dht.temperature().getEvent(&event);

  light = analogRead(lumPin);
  light = exp(0.02463*light-0.5522);

  Serial.print("Light: ");
  Serial.print(light);
  Serial.println(" Klux");
  String Light = String(light,1);
  client.publish(MQTT_LIGHT_PUBLISH_TOPIC.c_str(), Light.c_str());
  
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    temp = event.temperature;
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" *C");
    String Temp = String(temp,1);
    client.publish(MQTT_TEMP_PUBLISH_TOPIC.c_str(), Temp.c_str());
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
    client.publish(MQTT_HUMID_PUBLISH_TOPIC.c_str(), Humid.c_str());
  }



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

bool checkMqttConnection(){
  if (!client.connected()) {
    if (MQTT_AUTH ? client.connect(HOSTNAME.c_str(),MQTT_USERNAME, MQTT_PASSWORD) : client.connect(HOSTNAME.c_str())) {
      //SUBSCRIÇÃO DE TOPICOS
      Serial.println("CONNECTED");
      client.subscribe(MQTT_SYSTEM_CONTROL_TOPIC.c_str());

      client.publish(MQTT_LOG.c_str(),"Hello Sensor Module to Server");
    }
  }
}


  void loop() {

    if (WiFi.status() == WL_CONNECTED) {
      if (checkMqttConnection()){
        if (mytimer.onTimeout(delayMS)){
          publica();
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
    alerta();
  }

  void setupOTA(){
    if (WiFi.status() == WL_CONNECTED && checkMqttConnection()) {
      client.publish(MQTT_LOG.c_str(),"OTA SETUP ON");
      ArduinoOTA.setHostname(HOSTNAME.c_str());
      ArduinoOTA.setPassword((const char *)OTA_PASSWORD);

      ArduinoOTA.onStart([]() {
        client.publish(MQTT_LOG.c_str(),"START");
      });
      ArduinoOTA.onEnd([]() {
        client.publish(MQTT_LOG.c_str(),"END");
      });
      ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        String p = "Progress: "+ String( (progress / (total / 100)));
        client.publish(MQTT_LOG.c_str(),p.c_str());
      });
      ArduinoOTA.onError([](ota_error_t error) {
        if (error == OTA_AUTH_ERROR) client.publish(MQTT_LOG.c_str(),"Auth Failed");
        else if (error == OTA_BEGIN_ERROR)client.publish(MQTT_LOG.c_str(),"Auth Failed");
        else if (error == OTA_CONNECT_ERROR)client.publish(MQTT_LOG.c_str(),"Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)client.publish(MQTT_LOG.c_str(),"Receive Failed");
        else if (error == OTA_END_ERROR)client.publish(MQTT_LOG.c_str(),"End Failed");
      });
      ArduinoOTA.begin();
    }
  }

  void alerta(){

    if(humid < 60 || humid > 30){
      digitalWrite(green,HIGH);
      digitalWrite(red,LOW);
      digitalWrite(yellow,LOW);
    }
    if(humid >= 60) {
          digitalWrite(red,HIGH);
          digitalWrite(green,LOW);
          digitalWrite(yellow,LOW);
        }
    else{
      digitalWrite(yellow,HIGH);
        digitalWrite(green,LOW);
        digitalWrite(red,LOW);
      }

  }

