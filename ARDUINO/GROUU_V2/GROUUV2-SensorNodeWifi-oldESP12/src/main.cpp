#include "header.h"

void setup()
{
  Serial.begin(SERIAL_BAUDRATE);
  dht.begin();
  delayMS = 30000;

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
// Chamada de recepção de mensagem
void callback(char *topic, byte *payload, unsigned int length)
{
  String payloadStr = "";
  for (int i = 0; i < length; i++)
  {
    payloadStr += (char)payload[i];
  }
  Serial.println(payloadStr);
  String topicStr = String(topic);
  if (topicStr.equals(MQTT_SYSTEM_CONTROL_TOPIC))
  {
    if (payloadStr.equals("OTA_ON"))
    {
      OTA = true;
      OTABegin = true;
      Serial.println("//////////// OTA ON //////////////");
    }
    else if (payloadStr.equals("OTA_OFF"))
    {
      OTA = true;
      OTABegin = true;
      Serial.println("//////////// OTA OFF //////////////");
    }
    else if (payloadStr.equals("REBOOT"))
    {
      ESP.restart();
    }
  }
}

// Verifica se a ligação está ativa, caso não este liga-se e subscreve aos tópicos de interesse
bool checkMqttConnection()
{
  if (!client.connected())
  {
    if (MQTT_AUTH ? client.connect(Host.c_str(), MQTT_USERNAME, MQTT_PASSWORD) : client.connect(Host.c_str()))
    {
      // SUBSCRIÇÃO DE TOPICOS
      Serial.println("CONNECTED ON MQTT");
      client.subscribe(MQTT_SYSTEM_CONTROL_TOPIC.c_str());
      // Envia uma mensagem por MQTT para o tópico de log a informar que está ligado
      client.publish(MQTT_LOG.c_str(), (String(Host) + " CONNECTED").c_str());
    }
  }
  return client.connected();
}

// Setup do OTA para permitir updates de Firmware via Wi-Fi
void setupOTA()
{
  if (WiFi.status() == WL_CONNECTED && checkMqttConnection())
  {
    client.publish(MQTT_LOG.c_str(), (String(Host) + " OTA IS SETUP").c_str());
    ArduinoOTA.setHostname(Host.c_str());
    ArduinoOTA.setPassword((const char *)OTA_PASSWORD);
    ArduinoOTA.begin();
    client.publish(MQTT_LOG.c_str(), (String(Host) + " OTA IS READY").c_str());
  }
}

void publishValues()
{

  JsonDocument doc;
  char output[80];

  float newT = dht.readTemperature();
  float newH = dht.readHumidity();

  if (isnan(newT))
  {
    Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
    doc["t1"] = newT;
    doc["h1"] = newH;

    serializeJson(doc, output);
    Serial.println(output);

    client.publish(MQTT_room_sensors_PUBLISH_TOPIC.c_str(), output);
  }
  Serial.println("////////////////////");
}

void loop()
{

  if (WiFi.status() == WL_CONNECTED)
  {
    if (checkMqttConnection())
    {
      if (mytimer.onTimeout(delayMS))
      {
        publishValues();
      }
      client.loop();
      if (OTA)
      {
        if (OTABegin)
        {
          setupOTA();
          OTABegin = false;
        }
        ArduinoOTA.handle();
      }
    }
  }
}
