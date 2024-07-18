---
layout: default
title: GROUU DOCS
---


# ![alt text](./images/logo.png)
#### Modular Open Agriculture

Enabling the implementation of Precision agriculture / Automation in any context or scale of production:
- from indoor hydroponics to traditional agriculture;
- from a balcony vase implementation to a large exploration;
- in an urban or rural context;
These modules can be useful for all, used together, adapted, remixed, distributed...

#### Your collaboration is precious! No onboarding needed, just start designing and developing!

# GROUU V2 Documentation

## Server Documentation

# GROUU server V1 configuration
(GROUU stack V1)

This GROUU Stack V1 provides a solid foundation for your open-source IoT farming project:

1. Mosquitto for MQTT messaging between your farming sensors and the system
2. Node-RED for creating workflows and processing data from your farm
3. InfluxDB for storing time-series data from your agricultural sensors
4. Grafana for creating dashboards to visualize your farming data

# GROUU Stack V1 Installation and Deployment Tutorial

This guide will help you set up the GROUU Stack V1, an open-source IoT solution for personal and small-scale farming.

## 1. Install Docker

First, we need to install Docker on your Linux system:

```bash
# Update and upgrade the system
sudo apt update && sudo apt upgrade -y

# Install required packages
sudo apt install -y apt-transport-https ca-certificates curl gnupg lsb-release

# Add Docker's official GPG key and set up the repository
curl -fsSL <https://get.docker.com> -o get-docker.sh
sudo sh get-docker.sh

# Add your user to the docker group
sudo usermod -aG docker $USER

# Enable Docker to start on boot
sudo systemctl enable docker

```

Log out and log back in for the group changes to take effect.

## 2. Install Portainer

Now, let's install Portainer:

```bash
# Create volume for Portainer
docker volume create portainer_data

# Install Portainer CE
docker run -d -p 8000:8000 -p 9000:9000 --name=portainer --restart=always \\
  -v /var/run/docker.sock:/var/run/docker.sock \\
  -v portainer_data:/data \\
  portainer/portainer-ce

```

## 3. Access Portainer

Open a web browser and navigate to `http://your-server-ip:9000`. You'll be prompted to create an admin user.

## 4. Deploy the GROUU Stack V1

1. In Portainer, go to "Stacks" and click "Add stack".
2. Name your stack "GROUU-Stack-V1".
3. In the "Web editor" tab, paste the GROUU Stack V1 configuration:

```tsx
version: '3'

services:
  mosquitto:
    image: eclipse-mosquitto:latest
    ports:
      - "1883:1883"
    volumes:
      - mosquitto-config:/mosquitto/config
      - mosquitto-data:/mosquitto/data
      - mosquitto-log:/mosquitto/log
    restart: always

  nodered:
    image: nodered/node-red:latest
    ports:
      - "1880:1880"
    volumes:
      - nodered-data:/data
    restart: always

  influxdb:
    image: influxdb:latest
    ports:
      - "8086:8086"
    volumes:
      - influxdb-data:/var/lib/influxdb
    environment:
      - INFLUXDB_DB=grouudb
      - INFLUXDB_ADMIN_USER=admin
      - INFLUXDB_ADMIN_PASSWORD=grouuadmin  # Remember to change this password
    restart: always

  grafana:
    image: grafana/grafana:latest
    ports:
      - "3000:3000"
    volumes:
      - grafana-data:/var/lib/grafana
    restart: always

volumes:
  mosquitto-config:
  mosquitto-data:
  mosquitto-log:
  nodered-data:
  influxdb-data:
  grafana-data:

```

1. Click "Deploy the stack".

## 5. Post-Deployment Steps

After deploying the GROUU Stack V1, you can access its components:

1. Node-RED (for flow-based programming): `http://your-server-ip:1880`
2. InfluxDB (time-series database): Use the InfluxDB CLI or API at `http://your-server-ip:8086`
3. Grafana (for visualization): `http://your-server-ip:3000`
4. Mosquitto (MQTT broker): Use an MQTT client to connect to `your-server-ip:1883`

Remember to replace `your-server-ip` with your actual server IP address.

## 6. Security Considerations

- Change default passwords for all services in the GROUU Stack V1
- Use SSL/TLS for MQTT if exposing your farming data to the internet
- Set up proper firewalls and access controls
- Regularly update all containers and the host system to ensure your farming data remains secure

Congratulations! You now have the GROUU Stack V1 up and running for your personal or small-scale farming IoT project.

# … if you don’t want to use the stack config
GROUU STACK MANUAL INSTALATION

## 1. Install Docker

```bash
# Update and upgrade the system
sudo apt update && sudo apt upgrade -y

# Install required packages
sudo apt install -y apt-transport-https ca-certificates curl gnupg lsb-release

# Add Docker's official GPG key
curl -fsSL <https://download.docker.com/linux/raspbian/gpg> | sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg

# Set up the stable repository
echo "deb [arch=armhf signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] <https://download.docker.com/linux/raspbian> $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

# Install Docker Engine
sudo apt update
sudo apt install -y docker-ce docker-ce-cli containerd.io

# Add your user to the docker group
sudo usermod -aG docker $USER

# Enable Docker to start on boot
sudo systemctl enable docker

```

## 2. Install Portainer CE and Portainer Agent

```bash
# Create volume for Portainer
docker volume create portainer_data

# Install Portainer CE
docker run -d -p 8000:8000 -p 9000:9000 --name=portainer --restart=always -v /var/run/docker.sock:/var/run/docker.sock -v portainer_data:/data portainer/portainer-ce

# Install Portainer Agent
docker run -d -p 9001:9001 --name portainer_agent --restart=always -v /var/run/docker.sock:/var/run/docker.sock -v /var/lib/docker/volumes:/var/lib/docker/volumes portainer/agent

```

## 3. Install Containers via Portainer

Access Portainer web interface at `http://your-raspberry-pi-ip:9000` and follow these steps for each container:

### 3.1 Mosquitto (MQTT Broker)

1. Go to "Containers" > "Add container"
2. Name: mosquitto
3. Image: eclipse-mosquitto:latest
4. Port mapping: 1883:1883
5. Volumes:
    - /path/to/mosquitto/config:/mosquitto/config
    - /path/to/mosquitto/data:/mosquitto/data
    - /path/to/mosquitto/log:/mosquitto/log

### 3.2 Node-RED

1. Go to "Containers" > "Add container"
2. Name: nodered
3. Image: nodered/node-red:latest
4. Port mapping: 1880:1880
5. Volumes:
    - /path/to/nodered/data:/data

### 3.3 InfluxDB

1. Go to "Containers" > "Add container"
2. Name: influxdb
3. Image: influxdb:latest
4. Port mapping: 8086:8086
5. Volumes:
    - /path/to/influxdb/data:/var/lib/influxdb
6. Environment variables:
    - INFLUXDB_DB=mydb
    - INFLUXDB_ADMIN_USER=admin
    - INFLUXDB_ADMIN_PASSWORD=adminpassword

### 3.4 Grafana

1. Go to "Containers" > "Add container"
2. Name: grafana
3. Image: grafana/grafana:latest
4. Port mapping: 3000:3000
5. Volumes:
    - /path/to/grafana/data:/var/lib/grafana

## 4. Post-Installation Steps

1. Access Mosquitto: Use an MQTT client to connect to `your-raspberry-pi-ip:1883`
2. Access Node-RED: Open a web browser and go to `http://your-raspberry-pi-ip:1880`
3. Access InfluxDB: Use the InfluxDB CLI or API at `http://your-raspberry-pi-ip:8086`
4. Access Grafana: Open a web browser and go to `http://your-raspberry-pi-ip:3000`

Remember to replace `your-raspberry-pi-ip` with the actual IP address of your Raspberry Pi.

## 5. Security Considerations

- Change default passwords for all services
- Use SSL/TLS for MQTT if exposing to the internet
- Set up proper firewalls and access controls
- Regularly update all containers and the host system



# Modules V1 - ESP12 based version (under dev - design (boards, enclosures), firmware(arduino))

### Types:
- core;
- s modules (wireless sensors);
- a modules (actuator modules);

### Development Goals

- The way the module is powered is not yet fully defined: All should have batteries (3.3V _____ Amps), some should have only this as power source and be periodically charged (induction most probably), and others, can be connected to a power source (solar, network, wind...) for constant charging;
- A power source (ex: solar panel) can be a module on its own and an induction charger can be another module (tbd - to be designed);
- The following code:
```
const String Instalation = "myFarm"; //Where is it?
const String IDCODE = "1"; //number your probe
const String TYPE = "SoilProbe"; //choose type
const String Host = "Grouu" + Instalation + TYPE + IDCODE; //just change if it is not grouu and you're adapting the code for something else
const char * OTA_PASSWORD  = "yourpasswordhere";

```
will allow you to set a hostname (Host) - These are the only parameters you should have to change before injecting the firmware on your board (we should work on an even simpler method);

- This hostname (`const String Host`):
1. will be the name of the network the device generates each time it is not able to connect to any wireless network;
2. Will identify the MQTT addresses being used by that type of device (expl: `HostName/Sensor/Moist1`);

- There shouldn't be a limit for the ammount of modules developed nor used. The scale and context should dictate these options.
- All Explorations, independently from the context or scale should be considered valid data generators;

### Call for Collaboration

It is permanent, develop whatever you want whenever you want!


## soilProbe
![alt text](./images/soil.png)
- The soil probe reads Soil Moisture and Soil Temperature;
- You can add as many as you want in order to measure these in as many points as you wish.

### design
**eagleCAD:**
**Fusion 3D (plastic 3D print Enclosure):**

### code
**Arduino IDE Firmware:**


## waterRouter
![alt text](./images/router.png)
- The water router uses a standart 1 in 4 out electrovalves from a washing machine. You should also decrease the water pressure on the outputs for microirrigation.
- You can use this directly to the network or after a motor (be careful with the pressure in compatibility). You can add as many as you want in the system.

### design
**eagleCAD:**
**Fusion 3D** (plastic 3D print Enclosure):

### code
**Arduino IDE Firmware:**


## core
![alt text](./images/core.png)
- Core is the local server. Right now is mostly a test hub were the data from the networked servers is received and processed.
- We are using Node-Red but all collaboration is welcomed!

### design
Right now we are using a **standard Raspberry case.**

### code
**configure and install:**

#### configuring Raspberry Pi zero W as server

1. configuring the Pi
https://core-electronics.com.au/tutorials/raspberry-pi-zerow-headless-wifi-setup.html

2. Accessing through ssh on mac or linux terminal
https://www.raspberrypi.org/documentation/remote-access/ssh/unix.md

3. upgrade Node-Red and Node.js
- open terminal;
- commands:
```
sudo apt-get update
sudo apt-get upgrade
bash <(curl -sL https:/raw.githubusercontent.com/node-red/raspbian-deb-package/master/resources/update-nodejs-and-nodered)
sudo systemctl enable nodered.service
```
- Connect to Raspberry Pi Node-red console throught your browser be accessing ```http://"raspipaddress":1880```

- Install Mosquitto MQTT broker
```sudo apt-get install Mosquitto```

some instructions on how to test here: https://www.instructables.com/id/Installing-MQTT-BrokerMosquitto-on-Raspberry-Pi/

4. other useful links:
Security: manage permissions on node-red tutorial - https://www.youtube.com/watch?v=GeN7g4bdHiM




# Archived on this REPO You'll also find:

## mainSoilProbe REV0 - Developed at the Habibi Works (Ioannina, Greece) CultiMake Workshop - P2PLab - Archived
# ![alt text](./images/soilProbeHabibi.jpg)

### design
**eagleCAD:**
- A provisional board (not working!!) is on [/BOARDS/soilProbe/Archive](/BOARDS/soilProbe/Archive);
- For this example a nodeMCU dev board was used and tested at the workshop (see Habibi.Works [Workshop Journal](WORKSHOP/habibiWorksJuly2018/workshopJournal.md))
**enclosure:** A standard plastic box was used for demo purposes.

*This board includes:*
- 1 x 4 male header for DHT22 Temperature and Humidity Sensor
- 1 x 3 male header for DS18B20 (encapsulated version for Soil Temperature);
- 2 x 2 male header connections for Moisture 10k with Two probes;
- 1 led (no usage yet, mostly for debug purposes);
- 1 ftdi set headers;
- 1 voltage converter 12V - 3,3V;
- 1 x 2 screw headers to add solar panel + battery (maybe connect solar battery pack now).

### code
**Arduino IDE Firmware:** [/ARDUINO/habibiWorksJuly2018-WS/grouu-irr](/ARDUINO/habibiWorksJuly2018-WS/grouu-irr)


## GROUU greenhouse - Archived
# ![alt text](./images/grouuView.png)

### design
**SolidWorks files** for the complete Structure (to be added) - also check constructive system (levosystems.com)
**Photos** (soon)
**Other Diagrams** (soon)


### code
**Arduino Yun code:** [/ARCHIVE/arduino/GROUU_GREENHOUSE_0/](/ARCHIVE/arduino/GROUU_GREENHOUSE_0/)











## Refs:
#### solarpower
https://www.aki.pt/electricidade/producao-energia-renovavel/solar-eolico-energia-propia/acessorios-de-energias-renovaveis/bateria/controladores/painelmanutencaobaterias-p10533.aspx#info
https://www.aki.pt/electricidade/producao-energia-renovavel/solar-eolico-energia-propia/kits-solares/kitbombadeaguamicrosolar-p49581.aspx
https://mauser.pt/catalog/product_info.php?products_id=82391

Main Probe

#### Multiple Analogs -

https://www.instructables.com/id/Multiple-Analog-Inputs-on-Only-One-Analoge-Pin/

#### ESP12 + DHT22 -

https://cityos.io/tutorial/2006/ESP12-and-DHT22

#### ESP12  + temp

https://iot-playground.com/blog/2-uncategorised/41-esp8266-ds18b20-temperature-sensor-arduino-ide

#### Voltage Regulator

http://www.ti.com/lit/ds/symlink/lm3480.pdf

#### use FTDI on ESP12

https://www.hackster.io/harshmangukiya/program-esp8266-esp-12e-with-arduino-using-ftdi-cable-2310c9


### refs fabrication:
#### eagle to flatCAM

http://caram.cl/software/flatcam/board-cutout-with-flatcam/
