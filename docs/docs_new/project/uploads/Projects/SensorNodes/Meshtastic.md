---
tags: [GROUU, meshtastic, lora]
tipo: docs
---

# Meshtastic LoRa Mesh — GROUU

> Wireless sensor mesh using Meshtastic firmware on Seeed Studio hardware. Gateway bridges LoRa → WiFi → MQTT to the RPi server stack.

## Index

- [[#Hardware]]
- [[#Firmware]]
- [[#Data Flow]]
- [[#Gateway Node Config (ESP32-S3)]]
- [[#Field Sensor Node Config (nRF52840)]]
- [[#Node-RED Flow]]
- [[#Debug Commands]]
- [[#WiFi/MQTT vs USB/Docker]]
- [[#Known Issues]]
- [[#Reference Links]]

**See also:** [[../../Server/steam228IOTRPI4@HOME|Server overview]] · [[../../Server/HomeAssistant|Home Assistant]] · [[../../Server/TODO-Server|Pending tasks]]

---

## Hardware

| Role | Board | Radio | Connection |
|------|-------|-------|------------|
| **Gateway** | Xiao ESP32-S3 | Wio-SX1262 | WiFi → MQTT to Mosquitto |
| **Field sensor** | Xiao nRF52840 | Wio-SX1262 | LoRa mesh only (no WiFi) |

> Wio-SX1262 shields are **NOT interchangeable** between ESP32-S3 and nRF52840 kits — different B2B pinouts.

---

## Firmware

- Standard Meshtastic firmware (pre-flashed from Seeed) — no custom firmware needed
- Current version: **2.7.18** (updated from 2.7.15 to fix SHT40 NaN bug — [GitHub #7522](https://github.com/meshtastic/firmware/issues/7522))
- Update via [Meshtastic Web Flasher](https://flasher.meshtastic.org/)
- **Always connect antenna BEFORE powering on** (mandatory — protects the radio)

---

## Data Flow

```
nRF52840 field sensor nodes
    │ LoRa 868 MHz mesh
    ▼
Xiao ESP32-S3 gateway (WiFi)
    │ MQTT publish (JSON) over WiFi
    ▼
Mosquitto (localhost:1883)
    │ topic: GROUU/meshtastic/+/json/#
    ▼
Node-RED (parse, sort, clean)
    │
    ├──▶ InfluxDB (GROUUmeshtastic bucket) → Grafana
    └──▶ Home Assistant (MQTT integration)
```

Every new node added to the mesh shows up in InfluxDB with its unique sender ID (e.g., `!da74148c`) — **the system scales automatically**.

---

## Gateway Node Config (ESP32-S3)

Configure via Meshtastic iOS app over BLE.

**LoRa Config:**
- Region: `EU_868`

**Device Config:**
- Role: `CLIENT` or `ROUTER` (router if it should also relay for other nodes)

**Network Config:**
- WiFi SSID + password: your home network

**Module Config → MQTT:**
- Enabled: on
- Server: `192.168.1.231`
- Port: `1883`
- Username: `meshtastic` / Password: (as set during Mosquitto auth setup)
- Encryption Enabled: off
- JSON Enabled: **on** (translates LoRa radio into readable JSON)
- Root Topic: `GROUU/meshtastic/`
- Uplink Enabled: on (primary channel)
- Downlink Enabled: on (primary channel)

---

## Field Sensor Node Config (nRF52840)

Configure via Meshtastic iOS app over BLE.

**LoRa Config:**
- Region: `EU_868`

**Device Config:**
- Role: `CLIENT` or `SENSOR` (sensor role optimises for low power)

**Module Config → Telemetry:**
- Environment metrics: enabled
- Update interval: as needed (e.g., 300s = 5 min)

> The nRF52840 has no WiFi and does NOT need MQTT or network config. It sends data over LoRa mesh; the ESP32-S3 gateway hears it and bridges to MQTT automatically.

---

## Node-RED Flow

Node-RED acts as the sorting and cleaning layer between raw Meshtastic JSON and InfluxDB.

### Flow structure

```
mqtt in                    switch (msg.payload.type)        function (clean)       influxdb out
GROUU/meshtastic/   →  json  →  ├── "text"      → [text handler]    → GROUUmeshtastic bucket
  +/json/#                      ├── "position"   → [GPS handler]     → GROUUmeshtastic bucket
                                └── "telemetry"  → [battery/env]     → GROUUmeshtastic bucket
```

### Nodes

1. **mqtt in** — subscribes to `GROUU/meshtastic/+/json/#` (catches only JSON, ignores binary)
2. **json** — converts incoming string to JavaScript object
3. **switch** — routes by `msg.payload.type`:
   - `text` → text messages
   - `position` → GPS coordinates
   - `telemetry` → battery, environment metrics
4. **function** (cleaner) — extracts, rounds, and packages for InfluxDB
5. **influxdb out** — writes to `GROUUmeshtastic` bucket

### Example: telemetry function node

```javascript
// Meshtastic telemetry cleaner for InfluxDB
// Input:  msg.payload = parsed Meshtastic JSON object
// Output: msg.payload = [fields, tags] for influxdb-out node

var data = msg.payload;
var sender = data.sender || "unknown";
var telemetry = data.payload || {};

// Determine telemetry sub-type
var fields = {};
var measurement = "meshtastic_telemetry";

if (telemetry.temperature !== undefined) {
    // Environment metrics (SHT40 or similar)
    measurement = "meshtastic_environment";
    fields = {
        temperature:       Math.round(telemetry.temperature * 100) / 100,
        relative_humidity: Math.round(telemetry.relative_humidity * 100) / 100,
        barometric_pressure: telemetry.barometric_pressure || null
    };
} else if (telemetry.voltage !== undefined) {
    // Device metrics (battery, power)
    measurement = "meshtastic_device";
    fields = {
        battery_level:    telemetry.battery_level || 0,
        voltage:          Math.round(telemetry.voltage * 1000) / 1000,
        channel_utilization: Math.round((telemetry.channel_utilization || 0) * 1000) / 1000,
        air_util_tx:      Math.round((telemetry.air_util_tx || 0) * 1000) / 1000
    };
}

// Remove null fields
Object.keys(fields).forEach(function(k) {
    if (fields[k] === null || fields[k] === undefined) {
        delete fields[k];
    }
});

if (Object.keys(fields).length === 0) {
    return null; // drop empty payloads
}

var tags = {
    node: sender
};

msg.measurement = measurement;
msg.payload = [fields, tags];
return msg;
```

> Configure the **influxdb out** node: bucket `GROUUmeshtastic`, measurement from `msg.measurement`.

### Notes

- The `+` wildcard in the topic matches any node ID — new nodes are automatically ingested
- JSON output must be enabled on the gateway Xiao (Module Config → MQTT → JSON Enabled: on)
- If no JSON data appears, check that Uplink is enabled on the Primary Channel in the Meshtastic app

---

## Debug Commands

```bash
# Test MQTT output from gateway (from RPi host)
mosquitto_sub -h localhost -u iot_admin -P <password> -t "GROUU/meshtastic/+/json/#" -v

# Check all MQTT traffic (broad wildcard)
mosquitto_sub -h localhost -u iot_admin -P <password> -t "GROUU/#" -v
```

> No container to restart — the Xiao gateway is an independent WiFi device. To reconfigure it, use the Meshtastic iOS app via BLE.

---

## WiFi/MQTT vs USB/Docker

| | Original (USB/Docker) | Final (WiFi/MQTT) |
|---|---|---|
| Connection | Physical USB cable to Pi | Wireless (WiFi) |
| Middleman | `meshtasticd` Docker container | None |
| Data format | Serial stream (binary) | Clean JSON via MQTT |
| Complexity | High (USB passthrough, extra container) | Low (standard network traffic) |
| Pi resources | Higher CPU/RAM | Minimal |

---

## Known Issues

- **Dead SX1262 on ESP32-S3 kit:** The Wio-SX1262 shield reports `SX126x init result -2` — LoRa radio does not initialize. Persists after factory reset and firmware update. Needs replacement shield. Shields are NOT interchangeable between ESP32-S3 and nRF52840 kits (different B2B pinouts).

---

## Reference Links

- [Wio-SX1262 with Xiao ESP32S3 Kit](https://wiki.seeedstudio.com/wio_sx1262_with_xiao_esp32s3_kit/)
- [XIAO nRF52840 & Wio-SX1262 Kit for Meshtastic](https://wiki.seeedstudio.com/xiao_nrf52840&_wio_SX1262_kit_for_meshtastic/)
