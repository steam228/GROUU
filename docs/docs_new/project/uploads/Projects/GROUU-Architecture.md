---
tags: [grouu, architecture, evolution, overview]
github: "https://github.com/steam228/GROUU"
---

# 🌿 GROUU — Architecture & Evolution

← [[../HOME|🌿 HOME]] · [[MOC-Projects|🛠️ Projects]]

> The clearest single map of what GROUU is, how it evolved, and where each piece lives.

---

## Evolution Timeline

```
V0 (2012–2014) — The Centralized Greenhouse
│  Centralized Arduino-based automation for a controlled lab environment.
│  Concept: A "universal solution" for greenhouse management.
│
├──► V1 (2017–2021) — Modular Object Oriented Hardware
│    │  Focus: Breaking the monolithic greenhouse into independent, interconnectable nodes.
│    │  Philosophy: "Knowledge Transfer"—facilitating data exchange between humans and systems.
│    │  Tech Stack: Node-RED (logic), Fusion360 (CAD/CAM), Custom PCB (Soil Probe REV0).
│    │  Fabrication: 3D printing, laser-cut molds, epoxy-burlap composites.
│    │  → GitHub: GROUU/ARDUINO/GROUU_V1/
│
└──► V2 (2023–Present) — Situated Practice (Current)
     │  Focus: The designer's position in an Open Knowledge Ecosystem.
     │  Shift: From tech-development to intentional, situated research sites.
     │
     ├─── WiFi Sensor Node  ←── DEPLOYED at Loureiro (grouu01.local)
     │    → GitHub: GROUU/ARDUINO/GROUU_V2/GROUUV2-SensorNodeWiFi
     │
     ├─── LoRaWAN Sensor Node  ←── EVOLUTION track (farm-wide network)
     │    → GitHub: GROUU/ARDUINO/GROUU_V2/GROUUV2-SensorNodeLoRAWAN
     │
     └─── Server Stack
          → Mosquitto + Node-RED + InfluxDB + Grafana (Docker)
```

---

## The Two Sensor Network Approaches (V2) -> V3

|              | WiFi Nodes                  | LoRaWAN Nodes                          | Meshtastic? |
| ------------ | --------------------------- | -------------------------------------- | ----------- |
| **Status**   | ✅ Deployed                  | 🔬 In development                      |             |
| **Range**    | ~100m (router dependent)    | ~2-15km                                |             |
| **Power**    | Mains / USB                 | Battery + solar capable                |             |
| **Network**  | Local MQTT (grouu01.local)  | TTN (The Things Network) → MQTT bridge |             |
| **Best for** | Home / greenhouse / barn    | Outdoor farm, remote locations         |             |
| **GitHub**   | `GROUUV2-SensorNodeWiFi`    | `GROUUV2-SensorNodeLoRAWAN`            |             |
| **Note**     | [[SensorNodes/Sensor-Node-WiFi]] | [[SensorNodes/LoRaWAN]]                |             |

> **Key point:** LoRaWAN is not a replacement for WiFi nodes — it's a parallel architecture for contexts where WiFi coverage doesn't reach. Both feed into the same server stack.
> Also relevant: https://dev.to/vlad_avramut/lora-vs-lorawan-vs-meshtastic-the-architecture-most-people-get-wrong-288f

---

## Deployed Projects Using GROUU Hardware

| Project | Nodes | Status |
|---|---|---|
| [[GROUU-Setup-Loureiro\|Loureiro Farm]] | WiFi (deployed) | 🟡 In progress |
| [[Aquaponics/GROUU-Aquaponics\|Home Aquaponics]] | TBD | 🟡 In progress |
| [[Greenhouse/myGROUUgreenhouseFinal\|Greenhouse]] | WiFi (planned) | 🟡 In progress |

---

## GitHub Repositories

| Repo         | What's there                         | URL                                                                  |
| ------------ | ------------------------------------ | -------------------------------------------------------------------- |
| **GROUU**    | Main firmware, PCBs, workshops       | [github.com/steam228/GROUU](https://github.com/steam228/GROUU)       |


---

## 🔗 Related

- [[SensorNodes/LoRaWAN|GROUUWAN — LoRaWAN Evolution]]
- [[SensorNodes/Sensor-Node-WiFi|WiFi Sensor Node (deployed)]]
- [[Server/DOCS-HomeServer|Server Stack]]

---

## 🏷️ Tags

`#grouu` `#architecture` `#evolution` `#iot` `#wifi` `#lorawan` `#v1` `#v2`
