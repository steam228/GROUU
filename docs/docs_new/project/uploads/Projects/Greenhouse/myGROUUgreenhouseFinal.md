---
tags: [grouu, greenhouse, iot, automation, arduino, sensors, actuators]
status: in-progress
---

# 🥗 myGROUUgreenhouseFinal — Greenhouse Automation

← [[../MOC-Projects|🛠️ Projects]] · [[../../HOME|🌿 HOME]]

---

## What This Is

A small-scale automated greenhouse unit, part of the GROUU project. Designed for local sensor monitoring and actuator control — enclosed environment, not farm-wide sensing.

> **Note on communication scope:** The greenhouse uses **local wired/WiFi** communication. Long-range LoRaWAN sensing (for orchards and remote fields) is a separate GROUU evolution track → see [[../SensorNodes/LoRaWAN|📡 GROUUWAN]].

---

## Sensors

| Sensor | Quantity | Purpose |
|---|---|---|
| DHT22 (temp + humidity) | 1 | Air climate inside greenhouse |
| Temperature (additional) | 1 | Soil/root zone monitoring |
| Light (LDR) | 1 | Ambient light levels |
| Moisture | 2 | Soil moisture at dual depth |

---

## Actuators

| Actuator | Quantity | Purpose |
|---|---|---|
| Servo Latch | 1 | Vent / door control |
| Fan | 1 | Air circulation / cooling |
| Grow Lights (LED) | 2 | Supplemental growth lighting |
| Water Pump | 1 | Micro-irrigation |

---

## Local Sensor Details

→ [[Sensors|🔌 Greenhouse Sensor Specs]] — wiring, pin assignments, and local node setup

---

## Open Source Goals

- [ ] Upload structure + design files to **Wikifactory**
- [ ] Add **Open Know-How** metadata for discoverability
- [ ] Document fully for replication

---

## 🔗 Related

- [[../SensorNodes/LoRaWAN|GROUUWAN LoRaWAN]] — farm-wide LoRaWAN sensing (separate track)
- [[../GROUU-Architecture|GROUU Architecture]] — where greenhouse fits in the full evolution
- [[../Server/DOCS-HomeServer|Server Stack]] — backend for sensor data
- [[../SensorNodes/Sensor-Node-WiFi|Sensor Node WiFi]] — working WiFi node (deployed at Loureiro)

---

## 🏷️ Tags

`#greenhouse` `#iot` `#automation` `#grouu` `#sensors` `#actuators` `#arduino`
