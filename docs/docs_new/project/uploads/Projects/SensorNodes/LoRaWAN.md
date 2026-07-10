---
tags: [grouu, lorawan, ttn, grouuwan, sensors]
status: in-development
github: "https://github.com/steam228/GROUU/tree/master/ARDUINO/GROUU_V2/GROUUV2-SensorNodeLoRAWAN"
---

# LoRaWAN Sensor Network — GROUU

> Farm-wide, long-range, low-power sensing via LoRaWAN + The Things Network. Separate architectural track from WiFi nodes — both feed into the same [[steam228IOTRPI4@HOME|server stack]] via MQTT.

## Index

- [[#Why LoRaWAN]]
- [[#Architecture]]
- [[#Node Hardware Options]]
- [[#Firmware]]
- [[#TTN → MQTT Bridge]]
- [[#Naming Convention]]
- [[#Intended Deployments]]
- [[#Status]]

**See also:** [[Sensor-Node-WiFi|WiFi node (deployed)]] · [[Meshtastic|Meshtastic mesh]] · [[steam228IOTRPI4@HOME|Server overview]] · [[GROUU-Architecture|Architecture & Evolution]]

---

## Why LoRaWAN

| Need | WiFi | LoRaWAN |
|---|---|---|
| Long range (orchards, fields) | No | 2–15 km |
| Battery / solar powered | No (power hungry) | Years on battery |
| Dense sensor grid | Yes | Yes |
| Internet backbone (TTN) | Local only | Global network |
| Gateway needed | Router (existing) | LoRa gateway (new hardware) |

**Key point:** LoRaWAN is not a replacement for WiFi nodes — it's a parallel architecture for contexts where WiFi coverage doesn't reach.

Also relevant: [LoRa vs LoRaWAN vs Meshtastic — architecture comparison](https://dev.to/vlad_avramut/lora-vs-lorawan-vs-meshtastic-the-architecture-most-people-get-wrong-288f)

---

## Architecture

```
[Sensor Node: Arduino / ESP32 + RFM95 or MKR WAN]
           │  LoRaWAN (OTAA)
           ▼
[LoRa Gateway at farm]
           │  TTN (The Things Network)
           ▼
[TTN MQTT Bridge → steam228iot.local]
           │
           ▼
[Mosquitto → Node-RED → InfluxDB → Grafana]
(same server stack as WiFi + Meshtastic nodes)
```

---

## Node Hardware Options

| Board | Type | Notes |
|---|---|---|
| Arduino PRO mini + RFM95 | DIY | Lower cost, OTAA — [tutorial](https://electronicsinnovation.com/otaa-based-lorawan-end-node-with-arduino-nano-rfm95-how-to-make-lora-node/) |
| Lora Radio Node | Dev board | Ready-made |
| Arduino MKR WAN 1300 | Dev board | Built-in LoRa module |

---

## Firmware

**GitHub:** [`GROUU/ARDUINO/GROUU_V2/GROUUV2-SensorNodeLoRAWAN`](https://github.com/steam228/GROUU/tree/master/ARDUINO/GROUU_V2/GROUUV2-SensorNodeLoRAWAN)

PlatformIO project structure:

```
GROUUV2-SensorNodeLoRAWAN/
├── include/
├── lib/
├── src/           ← main firmware
├── test/
├── platformio.ini
└── .gitignore
```

---

## TTN → MQTT Bridge

Bridges The Things Network v3 to the local Mosquitto broker on the RPi server stack.

**Node-RED approach** (recommended for this stack):
1. Install `node-red-contrib-ttn` or use the MQTT-in node with TTN's MQTT integration
2. TTN Application → Integrations → MQTT — provides a broker URL + API key
3. Subscribe to `v3/{application-id}/devices/{device-id}/up` for uplink messages
4. Parse the decoded payload in a function node → write to InfluxDB

**Alternative — Mosquitto bridge:**
Add to `~/iot-stack/mosquitto/config/mosquitto.conf`:

```
connection ttn-bridge
address eu1.cloud.thethings.network:8883
topic v3/+/devices/+/up in 0 ttn/ GROUU/lorawan/
bridge_cafile /etc/ssl/certs/ca-certificates.crt
remote_username {application-id}@ttn
remote_password {api-key}
```

> Both approaches feed data into the same Node-RED → InfluxDB → Grafana pipeline used by WiFi and Meshtastic nodes.

---

## Naming Convention

Follows the GROUU standard: `Grouu[Installation][Type][ID]`

Examples:
- `GrouuLoureiro_Soil_1` → topic: `GrouuLoureiro_Soil_1/sensors`
- `GrouuLoureiro_Weather_1` → topic: `GrouuLoureiro_Weather_1/sensors`

---

## Intended Deployments

| Location | Sensors | Priority |
|---|---|---|
| Loureiro — orchard | Soil moisture (×8), weather | High |
| Loureiro — remote field | Soil moisture, temperature | Medium |
| Well — water quality (B1) | TDS, turbidity, pH, temperature | High |
| Greenhouse (if WiFi insufficient) | DHT, light, moisture | Low |

---

## Status

- [x] Hardware options researched
- [x] Firmware folder created on GitHub (PlatformIO)
- [x] TTN v3 bridge planned
- [x] LoRa gateway hardware acquired
- [x] First node tested on TTN
- [ ] Deployed at Loureiro

---

## Related

- [[GROUU-Architecture|Architecture & Evolution]] — where this fits in the GROUU timeline
- [[Sensor-Node-WiFi|WiFi Sensor Node]] — the parallel track (deployed)
- [[Meshtastic|Meshtastic Mesh]] — LoRa mesh alternative (no TTN dependency)
- [[steam228IOTRPI4@HOME|Server Stack]] — shared backend
- [[GROUU-Setup-Loureiro|Loureiro Setup]] — primary target deployment
- [[Refs|Refs]] — LoRa + TTN reference links
