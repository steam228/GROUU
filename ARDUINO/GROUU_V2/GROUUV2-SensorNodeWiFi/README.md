# GROUU V2 Sensor Node WiFi

## Secrets Management

This project uses a secrets management approach to keep sensitive information like WiFi credentials, MQTT server details, and OTA passwords out of the repository.

### Setup Instructions:

1. Copy the template file to create your own secrets file:
   ```
   cp include/secrets.h.template include/secrets.h
   ```

2. Edit the `include/secrets.h` file with your own credentials:
   ```c
   // WiFi credentials
   #define WIFI_SSID "your_wifi_ssid" 
   #define WIFI_PASSWORD "your_wifi_password"

   // MQTT configuration
   #define MQTT_BROKER_IP "your_mqtt_broker_ip"
   #define MQTT_BROKER_PORT 1883
   #define MQTT_AUTH true
   #define MQTT_USERNAME "your_mqtt_username"
   #define MQTT_PASSWORD "your_mqtt_password"

   // OTA configuration
   #define OTA_PASSWORD "your_ota_password"

   // Installation specific information
   #define INSTALLATION_NAME "YourLocation"
   #define DEVICE_ID "1"
   #define DEVICE_TYPE "sensorNode"
   ```

3. The `secrets.h` file is excluded from Git in the `.gitignore` file, so your credentials won't be committed to the repository.

## Project Description

This Arduino project is for a GROUU V2 Sensor Node that uses WiFi connectivity to publish sensor data to an MQTT server. It supports OTA (Over The Air) updates for remote firmware updates.

## Features

- DHT22 temperature and humidity sensor integration
- MQTT communication for sensor data publishing
- OTA (Over The Air) firmware updates
- WiFi connectivity 