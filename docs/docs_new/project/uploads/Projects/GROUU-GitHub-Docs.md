---
tags: [github, documentation, roadmap, grouu]
description: "Structure for the new GROUU GitHub documentation (v2-ready)"
---

# GROUU — Open Source Agriculture (GitHub Docs Structure)

> **"Understanding the situated designer in Open Knowledge Ecosystems."**

This document serves as the structure for the new `README.md` and `/docs` section of the [GROUU GitHub Repository](https://github.com/steam228/GROUU). It reflects the transition from a centralized hardware solution to a research-driven, situated practice.

---

## 1. Project Identity & Vision
*   **Tagline:** Situated Open Source Hardware for Distributed Design Research.
*   **Mission:** Investigating how designers and makers inhabit Open Knowledge Ecosystems through the lens of Open Source Agriculture.
*   **The Motto:** GROUU is not just a tool; it is a "Knowledge Transfer" medium between humans, plants, and systems.

---

## 2. Evolution Path (The Narrative)
*   **v0 (Centralized):** The Greenhouse as a Lab (Centralized Arduino automation).
*   **v1 (Modular):** "Object Oriented Hardware"—Distributed sensor networks (Node-RED + Fusion360).
*   **v2 (Situated):** Current phase. Small-scale, intentional nodes (WiFi/LoRaWAN) feeding into a research framework on Open Distributed Design (ODD).

---

## 3. System Architecture (The v2 Stack)

### **A. Hardware Nodes**
*   **GROUU WiFi:** ESP32-based nodes using MQTT/OTA for local deployments (e.g., Loureiro Farm).
*   **GROUUWAN (LoRaWAN):** Long-range sensing via The Things Network (TTN) for remote agricultural contexts.
*   **PCB Designs:** soilProbe (KiCad) and modular actuator drivers.
### **B. Data Pipeline (Docker Stack)**
*   **Ingestion:** Mosquitto MQTT Broker.
*   **Logic:** Node-RED (visual programming).
*   **Storage:** InfluxDB (Time-series data).
*   **Visualization:** Grafana (Dashboards for "success detection").

---
## 4. Fabrication & Tools
*   **CAD/CAM:** Parametric design via Fusion360.
*   **PCB:** KiCad for electronics design.
*   **Digital Fabrication:** 3D printing, CNC machining and experimental composites (epoxy-burlap).

---

## 5. How to Contribute / Replicate
*   **Setup:** Linking to `DOCS-HomeServer` for the Docker stack.
*   **Firmware:** Deployment guides for WiFi and LoRaWAN nodes.
*   **Open Know-How:** Adherence to metadata standards for discoverability.

---

## 6. License & Credits
*   **License:** (Likely CC-BY-SA and/or GPL).
*   **Author:** Andre Rocha
*   **Context:** Digital Media PhD research at FCT-UNL

---

## 🏷️ Tags
`#readme` `#documentation` `#grouu` `#phd` `#open-source-hardware`
