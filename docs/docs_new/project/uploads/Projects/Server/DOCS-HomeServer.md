---
tags: [grouu, server, docker, mqtt, influxdb, grafana, iot, homelab]
status: in-progress
---

# 🖥️ DOCS — HomeServer (GROUU Stack V1)

← [[../MOC-Projects|🛠️ Projects]] · [[../../HOME|🌿 HOME]]

---

## Stack Overview

The GROUU Stack V1 is an open-source IoT pipeline for small-scale farming data.

| Service | Role | Port |
|---|---|---|
| **Mosquitto** | MQTT broker — receives sensor data | 1883 |
| **Node-RED** | Flow-based processing + automation | 1880 |
| **InfluxDB** | Time-series database for sensor history | 8086 |
| **Grafana** | Dashboards + visualization | 3000 |
| **Portainer** | Container management UI | 9000 |

---

## Quick Access (on network)

```
Node-RED:   http://your-server-ip:1880
InfluxDB:   http://your-server-ip:8086
Grafana:    http://your-server-ip:3000
Mosquitto:  mqtt://your-server-ip:1883
Portainer:  http://your-server-ip:9000
```

---

## Docker Stack (Compose)

Deploy via Portainer → Stacks → Add Stack → name: `GROUU-Stack-V1`

```yaml
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
      - INFLUXDB_ADMIN_PASSWORD=CHANGE_THIS
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

---

## Installation Steps

### 1. Install Docker (Linux / Raspberry Pi)

```shell
sudo apt update && sudo apt upgrade -y
sudo apt install -y apt-transport-https ca-certificates curl gnupg lsb-release
curl -fsSL https://get.docker.com -o get-docker.sh
sudo sh get-docker.sh
sudo usermod -aG docker $USER
sudo systemctl enable docker
```

### 2. Install Portainer CE

```shell
docker volume create portainer_data
docker run -d -p 8000:8000 -p 9000:9000 --name=portainer --restart=always \
  -v /var/run/docker.sock:/var/run/docker.sock \
  -v portainer_data:/data \
  portainer/portainer-ce
```

### 3. Deploy Stack via Portainer UI

Portainer → Stacks → Add Stack → paste compose above → Deploy.

---

## InfluxDB Notes

### Get / Reset API Token

```shell
# Enter container shell
docker exec -it InfluxDB sh

# List auth tokens
influx auth list
```

---

## MQTT — TTN v3

See [[../SensorNodes/LoRaWAN#TTN → MQTT Bridge|LoRaWAN — TTN Bridge]] for full setup (Node-RED or Mosquitto bridge approaches).

---

## Security Checklist

- [ ] Change default passwords (all services)
- [ ] SSL/TLS for MQTT (if internet-exposed)
- [ ] Firewall rules
- [ ] Regular container updates

---

## 🔗 Related

- [[../SensorNodes/Sensor-Node-WiFi|Sensor Node WiFi]] — the node sending data to this stack
- [[../GROUU-Setup-Loureiro|GROUU Setup Loureiro]] — deployment location
- [[../Greenhouse/Sensors|Greenhouse Sensors]] — LoRa nodes (planned)
- [[../../Refs|Refs]] — related IoT references

---

## 🏷️ Tags

`#docker` `#mqtt` `#influxdb` `#grafana` `#node-red` `#homelab` `#grouu` `#iot` `#raspberry-pi`
