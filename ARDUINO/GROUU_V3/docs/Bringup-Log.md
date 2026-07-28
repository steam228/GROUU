# WellBouy — board bring-up log

Live log of bringing up the fabricated **well-bouy01** carrier board, stage by stage.

## Unit
- **2nd well-bouy01 board.** The 1st was ruined reworking the ADS1115 (lifted pad after
  repeated hand-reflows). This unit's ADS is good.
- USB-only during bring-up (bench). Board powers `v5v` from the XIAO's USB 5 V pin.

## Sensor set — as built on THIS unit
| Signal | Sensor | ADS ch | Terminal | Rail |
|---|---|---|---|---|
| Temp | DS18B20 (OneWire GPIO4) | — | U2 | 3V3 (always-on) |
| pH | Phidgets 1130 + ASP200 | A2 | U3 | 5 V switched (10k/18k divider) |
| TDS | **Seeed Grove TDS** | A0 | U4 | 5 V switched (direct) |
| Turbidity | **Seeed Grove turbidity** (⚠ replaces DFRobot SEN0189) | A1 | U5 | 5 V switched (10k/6.8k divider) |

> **Terminal→function map CONFIRMED against the PCB refdes** (layouts/default/default.kicad_pcb):
> U2=tb_ds, **U3=tb_ph**, U4=tb_tds, **U5=tb_turb**. The U-numbers do NOT follow schematic
> declaration order — atopile/KiCad annotated them independently, so the silk shows refdes,
> not function. Proven on the bench: turbidity plugged into U3 read A2 through the 10k/18k
> (0.643) pH divider (2204 mV ÷ 0.643 = 3.4 V; the 0.405 turbidity divider would imply an
> impossible 5.4 V). Correct terminal for turbidity is **U5 → A1**.

## Firmware changes made during bring-up (in the 01-WellBouy working copy)
1. **platformio.ini** — `lib_ldf_mode = deep+`. Fixes `SPI.h: No such file` when a filtered
   bring-up build compiles the global RadioLib/Adafruit-BusIO deps but the sketch doesn't
   `#include` them (LDF `chain` never reaches framework SPI/Wire).
2. **b01_lorawan** — added `node.setDutyCycle(false)` + `setDwellTime(false)` after join.
   Bench only: EU868 starts at DR0 (SF12); its ~150 s duty-cycle silence was longer than the
   30 s test interval, so every uplink returned `-1108 RADIOLIB_ERR_UPLINK_UNAVAILABLE`.
3. **b03/b04/b05** — added the **switched-5 V rail enable** in `setup()`
   (`pinMode(SENSOR_PWR_EN_PIN,OUTPUT); digitalWrite(...,HIGH); delay(SENSOR_WARMUP_MS)`).
   The analog bring-up sketches never powered the rail, so TDS/turbidity/pH read ~0.
   (prod already did this — the bug was bring-up-only, and the 1st board's dead ADS had
   masked it.)
4. **b_iscan** (temp diagnostic env) — 2-way I²C scanner (default + swapped SDA/SCL) with
   `lib_ignore` so it builds without RadioLib. Used to prove the 1st board's ADS silence
   wasn't a bus swap. Remove before final.

## Calibrations (this unit)
- **TDS** — `TDS_KVALUE 1.000 → 0.944`. Grove TDS in a **1413 µS/cm** standard read 748.8 ppm
  at k=1; target 707 ppm (1413 × 0.5, the 0.5 EC→TDS factor the firmware's DFRobot polynomial
  uses); k = 707 / 748.8 = 0.944 → **verified reads 705 ppm**. Cal at ~room temp; field
  readings self-correct via the DS18B20 temp-compensation.
- **Turbidity** — hardware ✓, **NTU curve still to fit**. Sensor swapped to Seeed Grove.
  Bench characterisation on this unit (U5 → 10k/6.8k → A1, 5 V rail):
  - clear water: A1 ≈ **1535 mV → sensor 3.79 V** (this is the Grove's 0-NTU anchor,
    vs the DFRobot 3.96 V the old code assumed — that mismatch is why clear read 582 NTU).
  - heavy milk (very murky): A1 ≈ **197 mV → sensor 0.49 V**.
  - Response is monotonic decreasing (dirtier = lower V), clean ~3.3 V of swing. The Grove
    IS a 5 V-range part, so the 10k/6.8k divider stays correct.
  `turbidity_sensor.cpp` still uses the DFRobot SEN0189 quadratic + `V_CLEAR 3.96` — invalid
  for the Grove (goes to 0 below ~2.5 V, as seen with the milk). Replace with a Grove mapping
  anchored at 3.79 V = 0 NTU. **Absolute NTU needs a formazin standard** we don't have on the
  bench yet; until then the honest output is clear-anchored relative turbidity. Structure the
  final driver to hold per-sensor cal (clear-water anchor + curve) so variants swap cleanly.
- **pH** — hardware ✓ (U3 → 10k/18k → A2, reads 1.665 V). Cal **carried over** from the
  previous bench rig (V4 0.987 / V7 1.519 / V10 1.980) — no buffers on the bench to re-fit.
  Path/divider identical to that rig, so it transfers; expect a small per-electrode offset.
  TODO: re-verify against pH 4/7/10 buffers when available.

## Flashing workflow (ESP32-S3 native USB)
- New board arrived with **prod** flashed → it deep-sleeps 30 min on boot → USB port vanishes
  after ~5 s. To flash: **bootloader dance** — hold **BOOT**, tap **RESET**, release **BOOT** →
  ROM download mode (port stays up).
- After every `pio upload`, esptool leaves the S3 in ROM download mode → **tap RESET** (BOOT
  high) to run the app. Bring-up sketches don't deep-sleep, so once running the port is stable.
- Serial read via a small pyserial script (pio's miniterm needs a TTY we don't have here);
  open with `dtr=False, rts=False` so it doesn't knock the chip back into download.

## Stage results
- **b01 LoRaWAN** — ✓ radio.begin, OTAA join, uplink sent (join intermittent ~2/3 = normal RF).
- **b02 temp** — ✓ DS18B20 27.7 °C, uplink sent.
- **b03 TDS** — ✓ ADS @ 0x48, 5 V rail + load switch, A0, **calibrated** (705 ppm @ 1413).
- **b04 turbidity** — hardware ✓ (Grove on U5→A1; clear 3.79 V → milk 0.49 V, clean swing).
  NTU curve fit pending (needs Grove mapping; formazin standard for absolute NTU).
- **b05 pH** — hardware ✓ (U3 → A2, 1.665 V). Cal carried over (no buffers); see above.
- **prod** — ✓ full cycle on the new board: ADS 0x48 → T 26.0 °C / TDS 615 ppm /
  Turb 3 idx (≈clear, Grove code sane) / pH 7.70 → OTAA join → 27-byte LPP uplink OK →
  deep sleep 1800 s. Board now running production firmware.

## TODO for the final code
- [x] Turbidity: Grove driver done — `TurbidityCal` per-sensor struct (GROVE active, SEN0189
      kept), clear-anchored relative index (3.79 V→0, 0.49 V→full-scale). prod builds clean.
      Still open: fit a real formazin NTU curve when a standard is available.
- [~] Lock per-unit cal constants (TDS k ✓; turbidity clear anchor ✓; **pH V4/V7/V10 still
      to re-verify against buffers**).
- [x] Support multiple sensor variants / per-sensor calibration — `TurbidityCal` struct.
- [x] Sync working-copy changes into `GROUU_V3` and commit (master `7e0fc11`, not pushed).
- [x] Remove the temporary `b_iscan` env — gone from platformio.ini + sketch deleted.
