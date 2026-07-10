---
tags: [wellbouy, lorawan, bringup]
---

# WellBouy — Bring-up Plan

Incremental bring-up: prove the radio first, then each sensor in isolation
(serial → TTN). `src/main.cpp` is the integration target, grown as we go.
Flash one stage at a time: `pio run -e <env> -t upload && pio device monitor`.

## Phase 0 — TTN setup (do once, before flashing)

> Full walk-through in [[README]] → "Register the device on TTN". Keys come from here and go into `secrets.h`.

- [ ] **Gateway online** — local gateway registered and *Connected* on TTN, EU868 frequency plan.
- [ ] **Create application** — TTN Console → *Applications* → *Create application*.
- [ ] **Register end device** — manual; freq plan EU868, **LoRaWAN 1.0.x**, **OTAA**.
- [ ] **Capture keys** — JoinEUI, DevEUI, AppKey (byte order **MSB**) → fill `include/secrets.h` (don't commit real keys).
- [ ] **Uplink payload formatter** — set to **CayenneLPP** (decodes all bring-up + prod payloads).

## Phase 1 — Firmware bring-up (one cpp at a time)

- [x] **01 · LoRaWAN comms** (`b01_lorawan`) — join TTN + send dummy counter. Proves radio wiring, keys, gateway, uplink path. ✅ 2026-06-26
      Joined as DevAddr 260BD83B; uplink `0100000202013A` decoded `digital_in_1=0, analog_in_2=3.14`, RSSI −37.
      Fixes: RadioLib 6.6.0 is 1.1-native → 1.0.x needs `beginOTAA(…, appKey, appKey)` (NULL nwkKey crashes).
      TTN: enabled **"Resets join nonces"** so repeated reflashes re-join (1.0.4 monotonic DevNonce).
      Flash w/ forced bootloader: unplug → hold **B** → replug → release **B**, then `pio run -e b01_lorawan -t upload`.
- [x] **02 · DS18B20 temp** (`b02_temp`) — read water temperature in isolation. ✅ 2026-06-26
	- [x] serial: clean °C — stable & responsive (drifted 28.1→27.9 while cooling)
	- [x] TTN: temp on TTN — uplink `01670107` → `temperature_1 = 26.3 °C`, RSSI −37
- [ ] **03 · Deep-sleep + power integration** (ADS-independent — next) — fold LoRaWAN + DS18B20 into `main.cpp` with ESP32 deep sleep + RTC session/nonce resume. Also **measure sleep & active current** (feeds solar-panel sizing).

> ⏸ **Reordered 2026-07-02** — analog stages deferred to the end. All of TDS/turbidity/pH run through the ADS, and the DFRobot ADS **died**: reverse-polarity via the DFRobot-Gravity ↔ Grove connector **pinout mismatch**. Confirmed dead (powered, bus healthy, wiring correct, **zero I²C ACK at any address** — verified with bit-bang probe). Buying a **Grove ADS1115** 2026-07-03 (Grove-to-Grove is keyed → no mismatch). Resume the stages below once it arrives.

- [x] **A · TDS** (`b03_tds`) — via **Grove ADS at 0x48** (replaced dead DFRobot unit). ✅ 2026-07-07
	- [x] serial: clean ppm — 0 (air) → **327 ppm** (tap water), stable & responsive
	- [x] TTN: ppm on TTN — join OK, uplink `luminosity_4 ≈ 327`. (Extra uplinks at 30 s hit EU868 duty-cycle — fine, prod interval is 600 s.)
- [x] **B · Turbidity** (`b04_turbidity`) — SEN0189 via 10k/6.8k divider → NTU, through ADS. ✅ 2026-07-08
	- [x] serial: clean NTU — clear water anchored to 0 NTU @ 3.96 V; milk → ~2975 NTU, stable
	- [x] TTN: NTU on TTN — on `luminosity_3` (Analog Input overflows >327). **Needs 5 V supply** — 3.3 V starves it.
- [x] **C · pH** (`b05_ph`) — Phidgets 1130 two-point cal (pH 4 & 7), through ADS via **10k/18.2k** divider. ✅ 2026-07-08
	- [x] serial: clean pH — two-point (pH7→1.591 V, pH4→1.078 V); pH7 buffer read 7.10. 1130 runs "acidic = lower V".
	- [x] TTN: pH on TTN — on `analog_in_2` (fits, 0-14).
	- [ ] ⚠ **electrode drift** — read 7.10→6.30 over ~20 min. Condition (soak pH7/KCl hrs) + recal before field.
- [x] **Final integration** (`prod`) — all 4 sensors via ADS + DS18B20 in `main.cpp`. ✅ 2026-07-08
      Reconcile vs stale scaffold: direct-ADC → **ADS1115** (ch0/1/2); turbidity `analog_in_3`→`luminosity_3` (overflow); `uplink()`→`sendReceive()` (fixes -1108); **`SENSOR_PWR_EN` GPIO5→GPIO1** (GPIO5 was the I²C SDA!).
      Deep sleep + RTC session/nonce resume; interval kept in RTC and **downlink-adjustable** (`01 <min>` = set interval, `02` = reboot). Class A ⇒ can't wake on demand; commands apply next wake.
	- [x] bench-validated: 2 wake cycles, **`resumed existing session`** (no re-join), all 4 fields, uplink OK. ✅ 2026-07-08
	- [x] downlink validated: `01 05` → `[DL] set interval -> 300 s`, next sleep 300 s. ✅ 2026-07-08
	      Detection bug fixed: RadioLib `downlink()` returns `ERR_NONE` (0) either way — test `dnLen>0`, NOT `state>0`.

> **Flashing a deep-sleeping node** (native USB drops during sleep, so a normal upload can't catch it):
> 1. Enter download mode: **hold B, tap R, release B** (or unplug → hold B → replug, keep B ~3 s). Port `/dev/cu.usbmodem*` appears and stays.
> 2. `pio run -e prod -t upload --upload-port /dev/cu.usbmodem2101` (explicit port; else PIO grabs a Bluetooth port).
> 3. esptool's "hard reset" does **not** exit USB-Serial/JTAG download mode — **replug the cable (no buttons)** to boot the app.
> Bench test uses `PLATFORMIO_BUILD_FLAGS="-DTX_INTERVAL_S=120"`; deployment build omits it (30 min default).

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
