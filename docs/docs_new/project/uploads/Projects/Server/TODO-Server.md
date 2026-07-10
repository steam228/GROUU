---
tags: [GROUU, server, todo]
tipo: todo
---

# TODO — steam228iot Server

> Pending tasks for the RPi 4 IoT stack. See [[steam228IOTRPI4@HOME|server overview]] for current state.

## Index

- [[#Infrastructure]]
- [[#Home Assistant]]
- [[#Grafana]]
- [[#Meshtastic]]
- [[#Backup]]

**See also:** [[steam228IOTRPI4@HOME|Server overview]] · [[HomeAssistant|Home Assistant]] · [[../SensorNodes/Meshtastic|Meshtastic nodes]]

---

## Infrastructure

### Enable cgroup memory tracking

Docker reports memory stats as 0 without this.

**Steps:**

1. Edit `/boot/firmware/cmdline.txt`:
   ```bash
   sudo nano /boot/firmware/cmdline.txt
   ```
2. Append to the **existing single line** (do NOT create a new line):
   ```
   cgroup_enable=memory cgroup_memory=1
   ```
3. Reboot:
   ```bash
   sudo reboot
   ```
4. Verify:
   ```bash
   docker stats --no-stream
   # MEM USAGE column should now show real values
   ```

- [x] Enable cgroup memory tracking

---

## Home Assistant

### Configure HA → InfluxDB integration

Sends entity state changes to InfluxDB for long-term storage and Grafana dashboards. Full guide in [[HomeAssistant#HA → InfluxDB Integration]].

**Steps:**

1. Edit `~/homeassistant/config/configuration.yaml` — add the `influxdb:` block (see [[HomeAssistant#HA → InfluxDB Integration|guide]])
2. Restart HA: `docker restart homeassistant`
3. Verify in InfluxDB UI → Data Explorer → `homeassistant` bucket

- [ ] Configure HA → InfluxDB integration

### Set up Node-RED ↔ MQTT ↔ HA flows

Connect Node-RED to Home Assistant for automation flows that combine MQTT sensor data with HA entities.

**Steps:**

1. In HA: create a long-lived access token (Profile → Security → Long-Lived Access Tokens)
2. In Node-RED: install `node-red-contrib-home-assistant-websocket`
3. Add HA server config node: URL `http://localhost:8123`, access token from step 1
4. Build flows that read/write HA entities

- [ ] Set up Node-RED ↔ MQTT ↔ HA flows

---

## Grafana

### Change default password

The Grafana admin account still uses the default password.

**Steps:**

1. Open `http://steam228iot.local:3000`
2. Log in as `admin` / current password
3. Go to: Profile (bottom-left avatar) → Change password
4. Set a strong password and update the [[steam228IOTRPI4@HOME#Credentials|credentials table]]

- [x] Change Grafana default password

### Add InfluxDB as Grafana data source

Full guide in [[HomeAssistant#InfluxDB as Grafana Data Source]].

**Steps:**

1. In Grafana → Connections → Data Sources → Add → InfluxDB
2. Query language: Flux
3. URL: `http://influxdb:8086`, org: `homelab`, token: all-access, bucket: `homeassistant`
4. Add second source named `InfluxDB - Meshtastic`, bucket: `GROUUmeshtastic`
5. Save & Test each

- [x] Add InfluxDB as Grafana data source

### Build Grafana dashboard for Meshtastic data

Create a dashboard using the `InfluxDB - Meshtastic` data source.

**Suggested panels:**

- Temperature / humidity over time (per node)
- Battery voltage over time (per node)
- Channel utilization gauge
- Node status table (last seen, battery %)

- [ ] Build Grafana dashboard for Meshtastic data

---

## Meshtastic

### Order replacement Wio-SX1262 shield

The shield on the ESP32-S3 kit is dead (`SX126x init result -2`). Shields are NOT interchangeable between ESP32-S3 and nRF52840 kits (different B2B pinouts).

- [ ] Order replacement Wio-SX1262 shield for ESP32-S3 kit

### Update gateway MQTT server IP

After the static IP change, the Meshtastic gateway Xiao still needs its MQTT server address updated.

**Steps:**

1. Connect to the ESP32-S3 via Meshtastic iOS app (BLE)
2. Module Config → MQTT → Server: `192.168.1.231`
3. Save and reboot node

- [ ] Update Meshtastic MQTT server IP to `192.168.1.231`

### Configure nRF52840 telemetry intervals

Set appropriate telemetry update intervals on the field sensor node.

**Steps:**

1. Connect to the nRF52840 via Meshtastic iOS app (BLE)
2. Module Config → Telemetry → Environment metrics: enabled
3. Set update interval (e.g., 300s = 5 min for field use, 60s for testing)
4. Save and reboot node

- [ ] Configure nRF52840 telemetry intervals

---

## Backup

### Set up Docker volume backup strategy

Tar-based cron backup covering all bind-mount data on the RPi.

**1. Create `~/backup-docker.sh`:**

```bash
#!/bin/bash
# Docker volume backup — steam228iot
# Backs up all service data directories to a timestamped tarball

BACKUP_DIR="/home/$USER/backups"
TIMESTAMP=$(date +%Y%m%d-%H%M)
BACKUP_FILE="$BACKUP_DIR/iot-backup-$TIMESTAMP.tar.gz"

mkdir -p "$BACKUP_DIR"

echo "[$(date)] Starting backup..."

# Stop services that write to disk (avoids corruption)
cd ~/iot-stack && docker compose stop influxdb grafana nodered

# Tar all service data
tar -czf "$BACKUP_FILE" \
  -C /home/$USER \
  iot-stack/mosquitto \
  iot-stack/nodered \
  iot-stack/influxdb \
  iot-stack/grafana \
  homeassistant/config \
  2>/dev/null

# Restart services
cd ~/iot-stack && docker compose start influxdb grafana nodered

# Keep only last 7 backups
ls -t "$BACKUP_DIR"/iot-backup-*.tar.gz | tail -n +8 | xargs rm -f 2>/dev/null

SIZE=$(du -h "$BACKUP_FILE" | cut -f1)
echo "[$(date)] Backup complete: $BACKUP_FILE ($SIZE)"
```

**2. Make executable:**

```bash
chmod +x ~/backup-docker.sh
```

**3. Schedule via cron (daily at 03:00):**

```bash
crontab -e
# Add this line:
0 3 * * * /home/$USER/backup-docker.sh >> /home/$USER/backups/backup.log 2>&1
```

**4. (Optional) Copy to NAS:**

If you have an NFS/SMB share mounted (e.g., at `/mnt/nas`), add after the tar step:

```bash
cp "$BACKUP_FILE" /mnt/nas/rpi-backups/
```

**5. Restore from backup:**

```bash
cd ~/iot-stack && docker compose down
docker stop homeassistant

cd /home/$USER
tar -xzf ~/backups/iot-backup-YYYYMMDD-HHMM.tar.gz

cd ~/iot-stack && docker compose up -d
docker start homeassistant
```

> For a more automated approach, consider [docker-volume-backup](https://github.com/jareware/docker-volume-backup) which runs as a container itself and supports S3, rotation, and notifications.

- [x] Set up backup strategy for Docker volumes
