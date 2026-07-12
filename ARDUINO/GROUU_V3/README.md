# GROUU V3 — WellBouy · Well-Water-Quality LoRaWAN Node

A battery-powered well-water-quality monitor (the board is solar-capable). A **Seeed XIAO ESP32-S3** + **Wio-SX1262**
LoRa node reads four water sensors, uplinks them to **The Things Network** (EU868,
OTAA, CayenneLPP), and deep-sleeps between readings. On a **Raspberry Pi**, the data
flows TTN → Node-RED → InfluxDB → Grafana, and a small **public dashboard**
(`webapp/`) renders it.

**Live dashboard → https://grouuwell01.hacktoimprove.com**

## System at a glance

```
XIAO ESP32-S3 + Wio-SX1262
  DS18B20 (temp) + ADS1115 [ A0 TDS · A1 turbidity · A2 pH ]
        │  CayenneLPP · OTAA · EU868 · deep sleep (30 min)
        ▼
The Things Network  ──(MQTT/TLS)──►  Raspberry Pi  ~/iot-stack
                                       Mosquitto · Node-RED · InfluxDB 2 · Grafana
                                            │
                                            ▼
                                       webapp/ (Express)  ──►  Cloudflare Tunnel
                                                                grouuwell01.hacktoimprove.com
```

## Repo layout

```
platformio.ini            Firmware build — envs: prod + b01..b05 bring-up + inject
include/
  config.h                Pins, ADS channels, timing, calibration, downlink opcodes
  secrets.h               LoRaWAN keys — GITIGNORED (copy from secrets.h.example)
  secrets.h.example        Template
src/
  main.cpp                prod: wake → read 4 sensors → uplink → deep sleep
  sensors/                temp / tds / turbidity / ph drivers (each has *FromMilliVolts)
  bringup/                one isolated sketch per stage (01 LoRa … 05 pH) + inject
server/                   Data pipeline: TTN formatter, Node-RED flow, Grafana, README
webapp/                   Public dashboard (Express + InfluxDB) — deployed on the Pi
hardware/well-bouy01/     Dedicated round PCB (atopile): XIAO + ADS + dividers + power
docs/                     Obsidian planning: Bringup-Plan.md, Board-Design.md
```

## Hardware

The four analog sensors are read through an **external I²C ADS1115** (16-bit),
powered at 3.3 V — *not* the XIAO's own ADC. 5 V sensor outputs are divided down
to stay under the ADS's 3.3 V input limit.

| Function | Part | Connection | Notes |
|---|---|---|---|
| LoRa radio | Wio-SX1262 | SPI · NSS 41 / DIO1 39 / RST 42 / BUSY 40 | `Module(41,39,42,40)` |
| Water temp | DS18B20 | GPIO4 (1-Wire) | 4.7 kΩ pull-up |
| TDS | Seeed Grove TDS | ADS **A0** | 5 V; ~2.3 V out (direct, safe); DFRobot Gravity algorithm |
| Turbidity | DFRobot SEN0189 | ADS **A1** | **5 V** + **10k/6.8k** divider (÷0.405) |
| pH | Phidgets 1130 | ADS **A2** | 5 V + **10k/18k** divider |
| I²C | ADS1115 @ 0x48 | SDA GPIO5 / SCL GPIO6 | XIAO Grove I²C |
| 5 V rail enable | load switch | **GPIO1 / D0** | off during deep sleep |

> ⚠️ **5 V sensors need their divider before the ADS.** Turbidity and pH swing
> above 3.3 V — wire the divider **first**; a raw 5 V-range output kills the ADS.
> `SENSOR_PWR_EN_PIN` is **GPIO1** — *not* GPIO5/6 (those are the I²C bus).

## Firmware

Bring-up is incremental — one sensor per PlatformIO env, proven on serial then over
TTN, before folding into `prod`. See `docs/Bringup-Plan.md`.

| Env | What |
|---|---|
| `prod` | full node: 4 sensors, deep sleep, RTC session resume, downlink commands |
| `b01_lorawan` | radio + OTAA join + dummy uplink |
| `b02_temp` · `b03_tds` · `b04_turbidity` · `b05_ph` | one sensor each (`SEND_LORA` flag) |
| `b_inject` | temp + TDS + turbidity combined (pipeline testing) |

```bash
cp include/secrets.h.example include/secrets.h   # then fill in your keys (gitignored)

pio run -e prod -t upload          # flash production firmware
pio device monitor                 # serial @ 115200

# quick bench interval for testing (default is 30 min):
PLATFORMIO_BUILD_FLAGS="-DTX_INTERVAL_S=120" pio run -e prod -t upload
```

> **RadioLib is pinned to `6.6.0`** (the LoRaWAN API changed later and won't
> compile as-is); `platform = espressif32@6.13.0` is pinned too.

**Flashing a deep-sleeping node** (native USB drops during sleep): hold **B**, tap
**R**, release **B** to enter download mode → `pio run -e prod -t upload
--upload-port /dev/cu.usbmodemXXXX` → replug (no buttons) to boot the app.

## LoRaWAN payload (CayenneLPP)

| Ch | LPP type | Field (after formatter) | Value |
|---|---|---|---|
| 1 | Temperature | `temperature` | water °C |
| 2 | Analog Input | `ph` | pH 0–14 |
| 3 | Luminosity | `turbidity` | NTU (0–65535) |
| 4 | Luminosity | `tds` | ppm (0–65535) |
| 5 | GPS | — | fixed location (sent occasionally) |

> TDS and turbidity ride on **Luminosity** (0–65535) — Analog Input (±327.67) would
> overflow. The TTN uplink formatter (`server/ttn-uplink-formatter.js`) renames the
> raw LPP keys to `temperature`/`tds`/`turbidity`/`ph`.

**Downlink commands** (`handleDownlink`, Class A → applied on next wake). Send
`frm_payload` base64 on FPort 1:

| Bytes | Effect |
|---|---|
| `01 05` | interval → 5 min |
| `01 1E` | interval → 30 min |
| `02` | reboot |

## Calibration (`include/config.h`)
- **pH** — two-point (pH 7 → `PH_CAL_V7`, pH 4 → `PH_CAL_V4`), measured at the ADS
  pin so the fit absorbs the divider. This adapter runs *acidic = lower voltage*.
- **TDS** — DFRobot Gravity, temp-compensated to 25 °C via a two-point `TDS_KVALUE`.
- **Turbidity** — DFRobot SEN0189 quadratic, anchored to the measured clear-water
  voltage (`TURBIDITY_V_CLEAR`) and rescaled so clear water = 0 NTU.

## Data pipeline & dashboard
- **`server/`** — TTN MQTT → Node-RED → InfluxDB 2 (org `homelab`, bucket
  `wellbouy`) → Grafana. Setup in `server/README.md`.
- **`webapp/`** — public Express dashboard querying InfluxDB, deployed as a
  container in the Pi's `iot-stack` and exposed via Cloudflare Tunnel at
  **grouuwell01.hacktoimprove.com**. Setup + deploy in `webapp/README.md`.

## Secrets
Real credentials live **only** in `include/secrets.h` (LoRaWAN keys) and
`webapp/.env` (InfluxDB token) — **both gitignored**. Copy the matching `.example`
files and fill them in locally. Never commit real keys or tokens.

## Status
Firmware ✅ (4 sensors calibrated · deep sleep + session resume + downlinks) ·
data pipeline ✅ · dashboard ✅ live · dedicated PCB fabricated (bring-up pending) ·
enclosure + field deployment: next.
