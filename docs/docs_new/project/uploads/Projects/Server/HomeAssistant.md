---
tags: [GROUU, server, home-assistant]
tipo: docs
---

# Home Assistant — steam228iot

> Standalone Docker container on `steam228iot.local` (RPi 4). Migrated from HAOS on RPi 5 via backup restore.

## Index

- [[#Service Details]]
- [[#Update Procedure]]
- [[#Commands]]
- [[#HA → InfluxDB Integration]]
- [[#InfluxDB as Grafana Data Source]]
- [[#Notes]]

**See also:** [[steam228IOTRPI4@HOME|Server overview]] · [[TODO-Server|Pending tasks]] · [[../SensorNodes/Meshtastic|Meshtastic nodes]]

---

## Service Details

|||
|---|---|
|**URL**|`http://steam228iot.local:8123`|
|**Container**|`homeassistant`|
|**Image**|`ghcr.io/home-assistant/home-assistant:stable`|
|**Version**|2026.3.4|
|**Config**|`~/homeassistant/config`|
|**Network**|Host mode|
|**Zigbee**|ZHA via SkyConnect v1.0 USB adapter|
|**Run mode**|Standalone (not in compose)|

- Runs with `--privileged` for USB device access
- Add-ons are NOT supported — equivalent services run as separate containers
- HACS should be present in `custom_components/hacs`

---

## Update Procedure

```bash
docker pull ghcr.io/home-assistant/home-assistant:stable
docker stop homeassistant && docker rm homeassistant
docker run -d \
  --name homeassistant \
  --restart=unless-stopped \
  --privileged \
  --network=host \
  -e TZ=Europe/Lisbon \
  -v ~/homeassistant/config:/config \
  --device=/dev/serial/by-id/usb-Nabu_Casa_SkyConnect_v1.0_9ec8891c7bd8ed11a861776162c613ac-if00-port0:/dev/serial/by-id/usb-Nabu_Casa_SkyConnect_v1.0_9ec8891c7bd8ed11a861776162c613ac-if00-port0 \
  ghcr.io/home-assistant/home-assistant:stable
```

---

## Commands

```bash
# Restart
docker restart homeassistant

# View logs
docker logs homeassistant -f

# Stop
docker stop homeassistant

# Start
docker start homeassistant
```

---

## HA → InfluxDB Integration

Sends entity state changes to InfluxDB for long-term storage and Grafana dashboards.

**1. Edit `~/homeassistant/config/configuration.yaml`:**

```yaml
influxdb:
  api_version: 2
  host: steam228iot.local
  port: 8086
  token: <all-access-token>
  organization: homelab
  bucket: homeassistant
  ssl: false
  max_retries: 3
  include:
    entity_globs:
      - sensor.*
      - binary_sensor.*
      - climate.*
```

> Adjust `include` to match the entities you want to log. Use `exclude` to filter out noisy sensors.

**2. Restart Home Assistant:**

```bash
docker restart homeassistant
```

**3. Verify in InfluxDB UI** (`http://steam228iot.local:8086`) → Data Explorer → select `homeassistant` bucket → check for new measurements.

---

## InfluxDB as Grafana Data Source

**1. In Grafana** (`http://steam228iot.local:3000`):

- Go to: Connections → Data Sources → Add data source → **InfluxDB**
- Query language: **Flux**
- URL: `http://influxdb:8086` (internal Docker network name — Grafana and InfluxDB are in the same compose stack)
- Organization: `homelab`
- Token: (all-access token)
- Default bucket: `homeassistant`

**2. Add a second data source** for the Meshtastic bucket:

- Same settings as above, but:
- Name: `InfluxDB - Meshtastic`
- Default bucket: `GROUUmeshtastic`

**3. Save & Test** each data source to verify connectivity.

> Use the `homeassistant` source for HA entity data and the `meshtastic` source for LoRa sensor data.

---

## Notes

- MQTT integration: Settings → Integrations → MQTT → broker: `localhost`, port: `1883`
- InfluxDB integration: see [[#HA → InfluxDB Integration]] above
- Credentials in [[steam228IOTRPI4@HOME#Credentials|server credentials table]]
