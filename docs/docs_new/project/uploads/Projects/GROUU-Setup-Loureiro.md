---
tags: [grouu, farm, loureiro, iot, sensors, irrigation]
status: not-started
location: Loureiro (farm/land)
---

# 🏡 GROUU Setup — Loureiro

← [[MOC-Projects|🛠️ Projects]] · [[../HOME|🌿 HOME]]

---

## Overview

Farm / land setup at Loureiro. Larger scale than the home aquaponics — outdoor, multi-zone, sensor network planned.

The WiFi sensor node [[SensorNodes/Sensor-Node-WiFi|already deployed]] here (installation name: `"Loureiro"`, broker: `grouu01.local`).

---

## Zones

### 🌱 Seedlings
- 1× Nursery (Fab Academy)
- 1× Double nursery (Gastronorm format)

### 🌳 Orchard
- 4 sets of 2 (shallow + deep) **moisture sensors** — full soil profile per tree
- **Drip irrigation control** on all trees: 4 levels = 4 independent lines

### 🥬 Garden
*(to be planned)*

### 🌦️ Main / Weather Station
- Weather station monitoring ambient conditions

---

## IoT Architecture (planned)

```
Loureiro Nodes
  ├─ Moisture sensors (×8) ──► ESP32 WiFi ──► grouu01.local (MQTT)
  ├─ Drip irrigation valves (×4)
  └─ Weather station
             │
             ▼
    [[Server/DOCS-HomeServer|Server Stack]]  (Mosquitto → InfluxDB → Grafana)
```

---

## Status

- [x] WiFi sensor node deployed — [[SensorNodes/Sensor-Node-WiFi|Sensor Node WiFi Example]]
- [ ] Moisture sensor network (×8)
- [ ] Drip irrigation control
- [ ] Weather station
- [ ] Seedlings nurseries (×2)

---

## 🔗 Related

- [[Aquaponics/GROUU-Aquaponics|Aquaponics]] — home counterpart
- [[SensorNodes/Sensor-Node-WiFi|Sensor Node WiFi]] — deployed node code
- [[Server/DOCS-HomeServer|Server Stack]] — data pipeline
- [[Greenhouse/myGROUUgreenhouseFinal|Greenhouse IoT]] — sensor specs reference

---

## 🏷️ Tags

`#grouu` `#farm` `#loureiro` `#iot` `#moisture-sensor` `#irrigation` `#weather-station`
