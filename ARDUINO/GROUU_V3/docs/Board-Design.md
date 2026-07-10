---
tags: [wellbouy, hardware, kicad]
---

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
- 🔁 **ADS history:** the DFRobot Gravity I²C ADS1115 (the earlier "DFR0553" — it *is* the ADC, **not** a solar manager, so the solar-manager slot is still open) **died in bring-up**: reverse-polarity from the DFRobot-Gravity ↔ Grove **connector pinout mismatch** (they mate but VCC/GND differ). Confirmed dead — powered, bus healthy, wiring correct, **zero I²C ACK** (bit-bang verified). → Switching to a **Grove ADS1115** (Grove-to-Grove keyed = no mismatch), buying 2026-07-03.
- ⚠️ **Lessons → board TODO:** (1) **reverse-polarity protection** on every rail; (2) **keyed connectors**, and never mix Gravity↔Grove without checking pinout; (3) firmware **I²C bus-recovery / watchdog** — a bad device can hang the hardware `Wire` driver (bit-bang probe + bus-level check now live in `b03_tds` for debugging).

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
