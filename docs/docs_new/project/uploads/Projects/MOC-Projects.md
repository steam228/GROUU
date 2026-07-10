---
tags: [MOC, grouu, projects, iot, fabrication]
---

# 🛠️ Projects — Map of Content

← [[../HOME|🌿 HOME]]

---

## Overview

The applied side of GROUU — the actual farming, electronics, automation, and content creation work. 

Two physical contexts:
- **Home** — aquaponics unit for aromatics (urban, indoor)
- **Loureiro** — farm/land setup (outdoor, larger scale)

See [[GROUU-Architecture|🏗️ GROUU Architecture]] for the full evolution timeline (V1 → V2 WiFi + LoRaWAN).

---

## 📁 Active Projects

| Project | Location | Status | Description |
|---|---|---|---|
| [[Aquaponics/GROUU-Aquaponics\|Aquaponics]] | Home | 🟡 In Progress | Home aquaponics unit for aromatics, recovered fish tank + CNC structure |
| [[GROUU-Setup-Loureiro\|Farm Setup]] | Loureiro | 🔴 Not Started | Seedlings nursery, orchard irrigation, weather station |
| [[Greenhouse/myGROUUgreenhouseFinal\|Greenhouse IoT]] | — | 🟡 In Progress | Local automation: sensors (DHT/light/moisture), actuators (fan, servo, pump) |
| [[SensorNodes/LoRaWAN\|GROUUWAN — LoRaWAN]] | Loureiro (planned) | 🟠 In Development | Farm-wide LoRaWAN sensing via TTN — separate evolution track from WiFi nodes |
| [[Server/DOCS-HomeServer\|Server Stack]] | — | 🟡 In Progress | Mosquitto + Node-RED + InfluxDB + Grafana (Docker) |
| [[SensorNodes/Sensor-Node-WiFi\|WiFi Sensor Node]] | Loureiro | ✅ Deployed | ESP32 + MQTT + OTA — working node at Loureiro |
| [[YouTube/GROUU-Youtube\|YouTube Channel]] | — | 🟡 In Progress | Video series on GROUU — concept + production plan |

---

## 🏗️ System Architecture

```
Sensors (ESP32 WiFi nodes) ────► WiFi + MQTT ──────────────────┐
                                                                 │
Sensors (LoRaWAN nodes) ──► TTN (The Things Network) ──► MQTT ─┤
                                                                 ▼
                                              Mosquitto MQTT Broker
                                                     │
                                        ┌────────────┤
                                        ▼            ▼
                                    Node-RED      InfluxDB
                                                     │
                                                     ▼
                                                  Grafana (dashboards)
```

- **WiFi track** (deployed): [[SensorNodes/Sensor-Node-WiFi|Sensor Node WiFi]] — working at Loureiro
- **LoRaWAN track** (in development): [[SensorNodes/LoRaWAN|GROUUWAN]] — farm-wide, separate evolution
- **Full evolution map**: [[GROUU-Architecture|GROUU Architecture & Evolution]]
- **Server**: [[Server/DOCS-HomeServer|DOCS HomeServer]] (Docker, Portainer, Raspberry Pi)

---

## 🔗 Cross-links

- Archived sensor paths: [[../Archive/MOC-Archive|Archive]]
- IoT refs: [[../Refs|Refs]]

---

## 🏷️ Tags

`#grouu` `#projects` `#aquaponics` `#iot` `#mqtt` `#docker` `#farming` `#arduino` `#lorawan`
