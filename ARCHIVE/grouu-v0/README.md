# GROUU V0 — GrouuPro (greenhouse + fertigation)

**Status: archived.** Circa 2013–2014. Not in active development — kept as the
origin of the GROUU lineage.

GrouuPro was a built greenhouse with an automated **fertigation (ferti-rega)
cabinet** feeding four raised beds. It did rich **data collection**, **published**
readings to the cloud, and performed **local actuation** — but remote control and
closed-loop automation were never implemented; the project ended before them.

- **Sole author:** the GROUU maintainer ([github.com/steam228](https://github.com/steam228)).
- **Build assistance:** Rui Lopes.

## Architecture (as built — from the firmware & schematic)

**Sensing (the "probe"):** DHT22 (air temperature + humidity), LDR light
(calibrated with an `exp()` curve), analog soil moisture, analog leaf wetness,
DS18B20 soil temperature (OneWire), analog pH. Tank level sensors (`SNA_DP/FR`)
and a flow sensor (`FLUX_1`) on the fertigation side.

**Boards & comms:** an **Arduino Yún** as the network gateway (`Bridge.h` +
`Temboo.h`) with **Arduino Uno**-class probe boards. Inter-board is **I²C**
(`I2C_Anything`, central at address 42) in the multi-board sketches; the final
`fullprobe_TEMBOO` reads all sensors on the Yún directly.

**Publication:** **Temboo** (cloud middleware, discontinued ~2019) appended
timestamped rows to a **Google Spreadsheet ("GROUU_EXP")** via the AppendRow
Choreo — the spreadsheet was the log/plot "dashboard".

**Local actuation (only):** a top-hatch **servo** (ventilation), and **DC pumps /
motors** driven by PWM through **MOSFETs on perfboard** — the six peristaltic
nutrient-dosing pumps, the main irrigation pump, and electrovalves feeding the
four raised beds. **No custom PCB was ever designed.**

**Fertigation cabinet** (from `drawings/…Esquema Ligações GrouuPro.pdf`): a
four-level cabinet — electronics · 6 peristaltic pumps · water + ferti-rega tanks
with level sensors · valves, main pump, flow sensor → 4 raised beds + drains.

## The master plan (designed, never built)

`GROUU OUTLINE .pdf` lays out the full intended product. Only the sensing +
Temboo-logging half was reached; the rest was designed and shelved:

- **Architecture (p.10):** `4 probes → Arduino Yún (Node client) → Raspberry Pi +
  Node server → web front end`, multi-unit and multi-user — almost exactly what
  GROUU became in V2/V3.
- **App:** login &amp; profiles (admin/user roles); environmental definitions
  (temperature / humidity / photoperiod limits, µEinsteins); tests (per raised-bed
  experiments with goals, moisture-stress overrides, fertirrigation scheduling);
  watering solutions (six-product nutrient mixes); log &amp; annotation (autolog +
  photos + archive-to-PDF); and a real-time dashboard with anomaly alerts.

Remote control and automation were part of this plan and were never implemented.

## Lineage

The same **sense → publish → visualise** shape GROUU still uses — the parts just
changed: Yún → **Temboo → Google Sheets** here became **MQTT → Node-RED →
InfluxDB → Grafana** in V2/V3.

## Where the code and files are

- **Firmware (the real code):** [`firmware/`](firmware)
  — the Yún/probe/Temboo sketches (`fullprobe_TEMBOO`, `GROUU_CENTRAL_GDRIVE_VF`,
  `Servo_Escotilha_Topo`, `Bombas_beta0`, `read_pH`, …). Temboo/Google credentials
  in the `TembooAccount.h` / sketch files have been **redacted**.
- **`solidworks/`** — the reorganised, dependency-resolved SolidWorks model tree
  (master + `Dependencies/` + `Detached Modules/`), recovered and audited from a
  2017 bulk archive. See [`solidworks/README.md`](solidworks/README.md) for the
  full forensic audit; the master is the November 2013 state and five 2014
  subsystems were never integrated.
- **This folder (design archive):** `GROUU OUTLINE .pdf` (the envisioned master
  plan); `drawings/` (structure & connector details, the fertigation schematic,
  presentation, base models); `photos/` (web-sized build photos — full-res on
  backup); `MANIFEST.md` + `full-file-index.txt` cataloguing the full **1.5 GB**
  of CAD / renders / raw photos / Keynote that stay on the maintainer's OneDrive
  backup rather than in git.

## Licence

Part of GROUU — GPLv3 for code, CC-BY-SA 4.0 for hardware & docs (repo root
`LICENSE.txt`). Third-party material (Lexan/BWF supplier brochures, Arduino
libraries) retains its own licensing.
