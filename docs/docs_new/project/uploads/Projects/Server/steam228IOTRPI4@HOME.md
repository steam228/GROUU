
# RPi 4 IoT Stack — System Documentation

> **Host:** `steam228iot.local` (Raspberry Pi 4) **IP:** `192.168.1.231` (static, Ethernet only — WiFi disabled) **OS:** Raspberry Pi OS Bookworm Lite (64-bit) **Container engine:** Docker **Updated:** 2026-04-07

## Index

- [[#Credentials]]
- [[#Architecture Overview]]
- [[#Services]]
  - [[#Portainer]]
  - [[#Mosquitto (MQTT Broker)]]
  - [[#Node-RED]]
  - [[#InfluxDB]]
  - [[#Grafana]]
- [[#File Structure]]
- [[#Common Commands]]
- [[#Ports Summary]]
- [[#Guide — Mosquitto Authentication]]
- [[#Guide — Static IP for steam228iot]]

**Detailed docs:** [[HomeAssistant|Home Assistant]] · [[../SensorNodes/Meshtastic|Meshtastic nodes]] · [[TODO-Server|Pending tasks]]

---

## Credentials

| Service            | Username        | Password                           | Notes                                          |
| ------------------ | --------------- | ---------------------------------- | ---------------------------------------------- |
| **RPi OS (SSH)**   | `steam228`      | `________`                         | `ssh steam228@192.168.1.231`                   |
| **Home Assistant** | `________`      | `________`                         | `:8123` — see [[HomeAssistant]]                |
| **Portainer**      | `steam228`      | `S0/F}M!Hv+u24oVI6#g^EqXGZ?8)9D35` | `:9000`                                        |
| **Mosquitto**      | `iot_admin`     | `________`                         | Main MQTT admin                                |
| **Mosquitto**      | `nodered`       | `________`                         | Node-RED → MQTT                                |
| **Mosquitto**      | `homeassistant` | `________`                         | HA → MQTT                                      |
| **Mosquitto**      | `meshtastic`    | `________`                         | Xiao gateway → MQTT                            |
| **InfluxDB**       | `admin`         | `________`                         | `:8086` · org: `homelab`                       |
| **InfluxDB API**   | —               | token: `________`                  | All-access, generated via `influx auth create` |
| **Grafana**        | `admin`         | `)Dh=wu,?D68kN?i`                  | `:3000` · **change from default!**             |
| **Node-RED**       | —               | `________`                         | `:1880`                                        |

> Fill in the `________` fields with actual passwords. Keep this file local / vault-only — do not commit passwords to a public repo.

---
![[Attachments/Screenshot 2026-04-01 at 11.31.33.png]]
## Architecture Overview

```
steam228iot.local (RPi 4)
│
├── Home Assistant (standalone container) → [[HomeAssistant]]
│   └── SkyConnect Zigbee coordinator (USB)
│
├── Portainer (standalone container)
│   └── Manages all Docker containers via web UI
│
└── IoT Stack (docker-compose)
    ├── Mosquitto (MQTT broker)
    ├── Node-RED (flow automation + Meshtastic JSON parsing)
    ├── InfluxDB (time-series database)
    └── Grafana (dashboards & visualization)

Xiao ESP32-S3 + Wio-SX1262 (LoRa gateway) → [[../SensorNodes/Meshtastic]]
    └── Connects via WiFi → publishes MQTT directly to Mosquitto
```

---

## Services

### Home Assistant

See **[[HomeAssistant]]** for full config, update procedure, commands, and InfluxDB integration guides.

|||
|---|---|
|**URL**|`http://steam228iot.local:8123`|
|**Container**|`homeassistant`|
|**Run mode**|Standalone (not in compose)|
|**Zigbee**|ZHA via SkyConnect v1.0 USB|

---

### Portainer

|||
|---|---|
|**URL**|`http://steam228iot.local:9000`|
|**Container**|`portainer`|
|**Image**|`portainer/portainer-ce`|
|**Data**|`portainer_data` (Docker volume)|
|**Run mode**|Standalone (not in compose)|

- Web UI to manage all Docker containers, images, volumes, networks
- Has access to the Docker socket (`/var/run/docker.sock`)

---

### Mosquitto (MQTT Broker)

|||
|---|---|
|**Port**|`1883` (no web UI)|
|**Container**|`mosquitto`|
|**Image**|`eclipse-mosquitto:2`|
|**Config**|`~/iot-stack/mosquitto/config/mosquitto.conf`|
|**Data**|`~/iot-stack/mosquitto/data`|
|**Logs**|`~/iot-stack/mosquitto/log/mosquitto.log`|
|**Network**|Host mode|
|**Auth**|Enabled — users: `iot_admin`, `nodered`, `homeassistant`, `meshtastic`|
|**Run mode**|docker-compose (`~/iot-stack`)|

HA integration: Add MQTT integration in HA → broker: `localhost`, port: `1883`

---

### Node-RED

|||
|---|---|
|**URL**|`http://steam228iot.local:1880`|
|**Container**|`nodered`|
|**Image**|`nodered/node-red:latest`|
|**Data**|`~/iot-stack/nodered`|
|**Run mode**|docker-compose (`~/iot-stack`)|

- Mosquitto: `steam228iot.local:1883` (user: `nodered`)
- InfluxDB: `http://influxdb:8086` (Docker internal name)
- HA: via Home Assistant nodes (long-lived access token)
- Meshtastic parsing flow: see [[../SensorNodes/Meshtastic#Node-RED Flow]]

---

### InfluxDB

|||
|---|---|
|**URL**|`http://steam228iot.local:8086`|
|**Container**|`influxdb`|
|**Image**|`influxdb:2`|
|**Data**|`~/iot-stack/influxdb/data`|
|**Config**|`~/iot-stack/influxdb/config`|
|**Run mode**|docker-compose (`~/iot-stack`)|

|||
|---|---|
|**Username**|`admin`|
|**Org**|`homelab`|
|**Buckets**|`homeassistant` (HA data), `GROUUmeshtastic` (Meshtastic sensor data)|
|**API Token**|All-access token generated via CLI (`influx auth create`) — UI-generated tokens caused auth errors|

> Token was regenerated via `docker exec -it influxdb influx auth create ...`. The org name is case-sensitive.

---

### Grafana

|||
|---|---|
|**URL**|`http://steam228iot.local:3000`|
|**Container**|`grafana`|
|**Image**|`grafana/grafana:latest`|
|**Data**|`~/iot-stack/grafana`|
|**Run mode**|docker-compose (`~/iot-stack`)|

- Data source setup: see [[HomeAssistant#InfluxDB as Grafana Data Source]]
- Since Grafana and InfluxDB are in the same compose network, use container name `influxdb` as hostname

---

### Meshtastic LoRa Mesh

See **[[../SensorNodes/Meshtastic]]** for full hardware, firmware, node configs, Node-RED flow with example code, and debug commands.

|||
|---|---|
|**Gateway**|Xiao ESP32-S3 + Wio-SX1262|
|**Connection**|WiFi → MQTT to Mosquitto|
|**MQTT root topic**|`GROUU/meshtastic/`|
|**InfluxDB bucket**|`GROUUmeshtastic`|

---

## File Structure

```
~/
├── homeassistant/
│   └── config/                  ← HA configuration (standalone)
│       ├── configuration.yaml
│       ├── automations.yaml
│       ├── .storage/
│       └── custom_components/
│
├── iot-stack/
│   ├── docker-compose.yml       ← IoT stack definition
│   ├── mosquitto/
│   │   ├── config/
│   │   │   └── mosquitto.conf
│   │   ├── data/
│   │   └── log/
│   ├── nodered/
│   ├── influxdb/
│   │   ├── data/
│   │   └── config/
│   └── grafana/
│
└── (ha-backup-extract/ — deleted, migration verified)
```

---

## Common Commands

### IoT Stack (Mosquitto, Node-RED, InfluxDB, Grafana)

```bash
cd ~/iot-stack

# Start all
docker compose up -d

# Stop all
docker compose down

# View logs
docker compose logs -f

# Update all images
docker compose pull
docker compose up -d

# Restart a single service
docker compose restart nodered
```

### General Docker

```bash
# List running containers
docker ps

# List all containers (including stopped)
docker ps -a

# List images
docker images

# Clean up unused resources
docker system prune

# Check disk usage
docker system df
```

> HA commands → [[HomeAssistant#Commands]] · Meshtastic debug → [[../SensorNodes/Meshtastic#Debug Commands]]

---

## Ports Summary

| Port | Service | Protocol |
|------|---------|----------|
| 1883 | Mosquitto MQTT | TCP |
| 1880 | Node-RED | HTTP |
| 3000 | Grafana | HTTP |
| 8086 | InfluxDB | HTTP |
| 8123 | Home Assistant | HTTP |
| 9000 | Portainer | HTTP |

---

## Guide — Mosquitto Authentication

> **Status: DONE.** Auth is enabled with users `iot_admin`, `nodered`, `homeassistant`, `meshtastic`. Steps below for reference.

**1. Create a password file inside the running container:**

```bash
# Create password file with first user
docker exec -it mosquitto mosquitto_passwd -c /mosquitto/config/passwd iot_admin
# (enter password when prompted)

# Add additional users (no -c flag — appends)
docker exec -it mosquitto mosquitto_passwd /mosquitto/config/passwd nodered
docker exec -it mosquitto mosquitto_passwd /mosquitto/config/passwd homeassistant
docker exec -it mosquitto mosquitto_passwd /mosquitto/config/passwd meshtastic
```

**2. Edit `~/iot-stack/mosquitto/config/mosquitto.conf`:**

```
listener 1883
persistence true
persistence_location /mosquitto/data/
log_dest file /mosquitto/log/mosquitto.log

allow_anonymous false
password_file /mosquitto/config/passwd
```

**3. Restart Mosquitto:**

```bash
docker restart mosquitto
```

**4. Update all clients with credentials:**

| Client | Where to update |
|--------|----------------|
| Home Assistant | Settings → Integrations → MQTT → Configure → add username/password |
| Node-RED | MQTT broker node → Security tab → add username/password |
| Meshtastic gateway (Xiao) | Meshtastic iOS app → Module Config → MQTT → username/password |

**5. Test:**

```bash
# Should fail (no credentials)
mosquitto_pub -h localhost -t test -m "hello"

# Should succeed
mosquitto_pub -h localhost -t test -m "hello" -u iot_admin -P yourpassword
```

> Optional: add an ACL file (`acl_file /mosquitto/config/acl`) to restrict which users can access which topics. For a homelab this is usually overkill.

---

## Guide — Static IP for steam228iot

> **Status: DONE.** Static IP `192.168.1.231/24` set on `eth0` via NetworkManager. WiFi disabled for stability.

**RPi OS Bookworm uses NetworkManager (`nmcli`), NOT `dhcpcd`.** The old `dhcpcd.conf` method does not apply.

**Set static IP via nmcli:**

```bash
# Set the static address (must be done before changing method)
sudo nmcli connection modify "Wired connection 1" ipv4.addresses 192.168.1.231/24

# Set gateway
sudo nmcli connection modify "Wired connection 1" ipv4.gateway 192.168.1.1

# Set DNS
sudo nmcli connection modify "Wired connection 1" ipv4.dns "192.168.1.1 8.8.8.8"

# Switch from DHCP to manual
sudo nmcli connection modify "Wired connection 1" ipv4.method manual

# Apply changes
sudo nmcli connection up "Wired connection 1"
```

**Disable WiFi (recommended for server stability):**

```bash
sudo nmcli radio wifi off
```

> The RPi 4 WiFi controller (`mmc1` SDIO) causes intermittent hangs (`Controller never released inhibit bit(s)`) under sustained Docker workloads. Running Ethernet-only eliminates this issue.

**Verify:**

```bash
ip addr show eth0    # should show 192.168.1.231/24
ping -c 3 8.8.8.8   # should succeed
nmcli connection show "Wired connection 1" | grep ipv4
```

---

## TODO

See **[[TODO-Server]]** for all pending tasks with step-by-step instructions.
