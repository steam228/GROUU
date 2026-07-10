---

tags: [wellbouy, lorawan, bringup]

---
# WellBouy — Bring-up Plan

Incremental bring-up: prove the radio first, then each sensor in isolation
(serial → TTN). `src/main.cpp` is the integration target, grown as we go.
Flash one stage at a time: `pio run -e <env> -t upload && pio device monitor`.

## Phase 0 — TTN setup (do once, before flashing)

> Full walk-through in [[README]] → "Register the device on TTN". Keys come from here and go into `secrets.h`.

- [x] **Gateway online** — local gateway registered and *Connected* on TTN, EU868 frequency plan. ✅ 2026-06-26
      
      gateway EUI: a84041ffff1f7798 
      
- [x] **Create application** — TTN Console → *Applications* → *Create application*. ✅ 2026-06-26
- [x] **Register end device** — manual; freq plan EU868, **LoRaWAN 1.0.x**, **OTAA**. ✅ 2026-06-26
- [x] **Capture keys** — JoinEUI, DevEUI, AppKey (byte order **MSB**) → fill `include/secrets.h` (don't commit real keys). ✅ 2026-06-26

      JoinEUI: 000000000
      Dev EUI: 70B3D57ED007836F
      AppKey; DB3F3731F0EB4E41727549C36363F2B9
      
- [x] **Uplink payload formatter** — set to **CayenneLPP** (decodes all bring-up + prod payloads). ✅ 2026-06-26

## Phase 1 — Firmware bring-up (one cpp at a time)

- [x] **01 · LoRaWAN comms** (`b01_lorawan`) — join TTN + send dummy counter. Proves radio wiring, keys, gateway, uplink path. ✅ 2026-06-26
- [x] **02 · DS18B20 temp** (`b02_temp`) — read water temperature in isolation. ✅ 2026-06-26
	- [x] serial: clean °C ✅ 2026-06-26
	- [x] TTN: temp on TTN ✅ 2026-06-26
- [ ] **03 · TDS** (`b03_tds`) — TDS ppm in isolation (25 °C default, no DS18B20).
	- [ ] serial: clean ppm
	- [ ] TTN: ppm on TTN
- [ ] **04 · Turbidity** (`b04_turbidity`) — SEN0189 via 10k/6.8k divider → NTU.
	- [ ] serial: clean NTU
	- [ ] TTN: NTU on TTN
- [ ] **05 · pH** (`b05_ph`) — Phidgets 1130, two-point cal (pH 4 & 7).
	- [ ] serial: clean pH
	- [ ] TTN: pH on TTN
- [ ] **Integration** (`prod`) — fold validated drivers into `main.cpp`: all sensors + deep sleep + session resume.

> Bring-up cpps are kept — reused later to test the dedicated board.

## Phase 2 — Dedicated board (KiCad)

- [ ] Schematic — XIAO + Wio-SX1262, switched 5 V rail, dividers, Grove/headers.
- [ ] Layout + routing.
- [ ] Fab + assemble; re-run Phase 1 cpps on the board.

## Phase 3 — Enclosure

- [ ] Design enclosure (sensor glands, antenna, sealing).
- [ ] Print/fab + fit check.

## Phase 4 — Field assembly & test

- [ ] Assemble full node.
- [ ] End-to-end soak test on TTN; tune `TX_INTERVAL_S`, verify power budget.



# WellBouy — Board Design Notes

Keep-modular vs bring-on-board, one item per block, in bring-up order.
Companion to [[Bringup-Plan]]. ✅ decided · 🟡 mid-decision · ❓ needs info.

## 01 · MCU + LoRaWAN radio
- **Modules:** XIAO ESP32-S3 (socketed) + Wio-SX1262.
- **On-board:** header sockets, decoupling, BOOT/RST access, USB-C reachable.
- ✅ Both stay as modules (RF + USB + antenna already done/certified). Bare SX1262 only considered for a later rev.

## ADC (shared front-end)
- **Module:** ADS1115 16-bit I²C — Seeed Grove 4-ch (plug-and-play) or Adafruit PID 1085. Phidgets does **not** include an ADC, so this is required.
- **On-board:** ADS at 3.3 V, addr 0x48, I²C pullups; all sensor outputs scaled to ≤3.3 V before its inputs.
- ✅ Use ADS. ⚠️ Changes firmware pin map: analog sensors → ADS A0–A3, XIAO gains I²C, DS18B20 stays digital.

## 02 · Temperature — DS18B20
- **Module:** none (probe on cable).
- **On-board:** 4.7 kΩ pullup, probe connector, 3.3 V powered.
- ✅ Settled.

## 03 · TDS — DFRobot Gravity
- **Module:** DFRobot Gravity TDS conditioner board.
- **On-board:** 5 V (switched) feed, output divider to ADS, probe connector.
- 🟡 Module for rev A; integrating the AC-excitation front-end is a rev-B maybe.

## 04 · Turbidity — SEN0189
- **Module:** SEN0189 sensor head/driver.
- **On-board:** 10k/6.8k divider, 5 V (switched) feed, probe connector.
- ✅ Divider on-board; head stays cabled.

## 05 · pH — Phidgets 1130
- **Module:** Phidgets 1130 adapter (analog conditioner, 0–5 V out, pH7 ≈ 2.5 V).
- **On-board:** 5 V (switched) feed, output divider to ADS, BNC/probe connector.
- 🟡 Keep module rev A (gigaΩ front-end + guard is easy to get wrong); integrate later only if needed.

## Connectors (all probes)
- ✅ Screw terminals / IP-rated (steady closed well, no Grove needed in the field). Grove only for bench.

## Power — Solar panel + battery + rails
- **Rails:** 3.3 V always-on (MCU, radio, ADS, DS18B20) + 5 V **switched** (analog conditioners, on ~1 s/cycle via GPIO load switch).
- **Test-phase module:** ❓ **DFR0553** — confirm it has MPPT/solar input and which rails (5 V only vs 5 V+3.3 V). If it's a real Solar Power Manager, it's the power core.
- **Fallback charger:** 🟡 **DFR1026** (Li-ion + 5 V boost, no MPPT). ⚠️ Bench-only until verified its 5 V **auto-restarts after full battery cutoff** (button/latch risk = no field auto-recovery).
- **On-board (custom rev):** our own high-side load switch for the 5 V sensor branch; later swap module for low-Iq buck/boost.
- **Battery:** 🟡 leaning 1S Li-ion 18650 (LiFePO4 if cold). Share battery node with XIAO BAT for the 3.3 V.
- **Panel:** ❓ none yet — size **after** bring-up measures sleep/active current; pick voltage to match the chosen charger.
