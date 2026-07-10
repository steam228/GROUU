---
tags: [grouu, aquaponics, fabrication, cnc, arduino]
status: in-progress
location: home
---

# 🌱 GROUU Aquaponics

← [[../MOC-Projects|🛠️ Projects]] · [[../../HOME|🌿 HOME]]

---

## Concept

A small aquaponics unit for growing aromatics at home, built around a **recovered fish tank**.

Inspiration: [Aquapioneers](https://aquapioneers.io) open-source aquaponics project.

Also serves as a **demo piece** to showcase different CNC approaches (2D digital carpentry + 3D CAM) for the Fablab context.

---

## The Design

- **Structure:** CNC-fabricated parts
  - 2D digital carpentry (sheet cuts)
  - 3D CAM (molds for vases)
- **Insulation:** cork (CNC-milled)
- **Container:** recovered fish tank

---

## The Prototype

*(physical build in progress)*

---

## The Process

1. Recovered old fish tank
2. Designed CNC structure for cork insulation and vase molds (link missing)
3. Fabricated at the Fablab
4. Assembly + plumbing

---

## Code

*(IoT/automation code — to be integrated with [[../Server/DOCS-HomeServer|server stack]])*

---

## Electronics

*(Sensors to be added — see [[../Greenhouse/Sensors|Greenhouse Sensor Specs]] for reference node implementation)*

> **Precursor:** The [fishTank](https://github.com/steam228/fishTank) repo on GitHub was an earlier IoT experiment (NodeMCU + ESP-01 + 3D-printed automatic feeder) that preceded this aquaponics unit. It established the ESP-based sensor approach used throughout GROUU.

---

There is a Fusion 360 project aimed at using my old fishtank.

---

## 🔗 Related

- [[../GROUU-Setup-Loureiro|GROUU Setup Loureiro]] — outdoor farm counterpart
- [[../Greenhouse/myGROUUgreenhouseFinal|Greenhouse IoT]] — automation goals
- [[../Server/DOCS-HomeServer|Server Stack]] — data pipeline
- [[../SensorNodes/Sensor-Node-WiFi|Sensor Node WiFi]] — sensor code reference
- [[../GROUU-Architecture|GROUU Architecture]] — aquaponics in context of full evolution
- [GitHub: fishTank](https://github.com/steam228/fishTank) — hardware precursor (NodeMCU + 3D-printed feeder)
- [[../../Refs|Refs]] — IoT references

---

## 🏷️ Tags

`#aquaponics` `#cnc` `#fabrication` `#grouu` `#arduino` `#home` `#aromatics`
