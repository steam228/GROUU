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

## Lineage

The same **sense → publish → visualise** shape GROUU still uses — the parts just
changed: Yún → **Temboo → Google Sheets** here became **MQTT → Node-RED →
InfluxDB → Grafana** in V2/V3.

## Where the code and files are

- **Firmware (the real code):** [`../arduino/GROUU_GREENHOUSE_0/`](../arduino/GROUU_GREENHOUSE_0)
  — the Yún/probe/Temboo sketches (`fullprobe_TEMBOO`, `GROUU_CENTRAL_GDRIVE_VF`,
  `Servo_Escotilha_Topo`, `Bombas_beta0`, `read_pH`, …). Temboo/Google credentials
  in the `TembooAccount.h` / sketch files have been **redacted**.
- **This folder (design archive):** `drawings/` (structure & connector details,
  the fertigation schematic, the project presentation, base models); `photos/`
  (placeholders — see its README); `MANIFEST.md` + `full-file-index.txt`
  cataloguing the full **1.5 GB** of CAD / renders / raw photos / Keynote, which
  stay on the maintainer's OneDrive backup rather than in git.

## Licence

Part of GROUU — GPLv3 for code, CC-BY-SA 4.0 for hardware & docs (repo root
`LICENSE.txt`). Third-party material (Lexan/BWF supplier brochures, Arduino
libraries) retains its own licensing.
