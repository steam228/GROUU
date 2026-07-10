---
tags: [grouu, greenhouse, sensors, arduino, wiring]
---

# 🔌 Greenhouse Sensor Specs

← [[myGROUUgreenhouseFinal|🥗 Greenhouse]] · [[../../HOME|🌿 HOME]]

---

> This note covers **local greenhouse sensors** — components wired directly to the control node inside the greenhouse enclosure. For farm-wide LoRaWAN sensing → [[../SensorNodes/LoRaWAN|📡 GROUUWAN]].

---

## Sensor List

| Sensor | Model | Purpose | Notes |
|---|---|---|---|
| Temperature + Humidity | DHT22 | Climate monitoring | 1-wire, 3.3/5V |
| Temperature (extra) | DS18B20 or DHT11 | Root zone / secondary | Waterproof version for soil |
| Light | LDR / photoresistor | Ambient light level | Voltage divider circuit |
| Soil Moisture | Capacitive × 2 | Dual depth moisture | Preferred over resistive (corrosion) |

---

## Node Architecture (Local)

```
[DHT22 + LDR + Moisture × 2]
         │  Wired (GPIO)
         ▼
[Arduino / ESP32 — greenhouse controller]
         │  WiFi / Serial
         ▼
[Mosquitto → InfluxDB → Grafana]
(see [[../Server/DOCS-HomeServer|Server Stack]])
```

---

## Status

- [x] Sensor types identified
- [ ] Node wired and tested
- [ ] Pin assignments documented
- [ ] Data flowing to server

---

## 🔗 Related

- [[myGROUUgreenhouseFinal|Greenhouse overview]] — actuators and goals
- [[../SensorNodes/LoRaWAN|GROUUWAN LoRaWAN]] — farm-wide sensing (separate track)
- [[../SensorNodes/Sensor-Node-WiFi|Sensor Node WiFi]] — working reference implementation
- [[../Server/DOCS-HomeServer|Server Stack]] — MQTT + InfluxDB + Grafana

---

## 🏷️ Tags

`#sensors` `#greenhouse` `#arduino` `#dht22` `#moisture` `#grouu` `#iot`
