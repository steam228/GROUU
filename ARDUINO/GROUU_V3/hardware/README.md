# WellBouy — Rev-A Carrier Board (atopile 0.15.x → KiCad 10)

Code-defined electronics with **atopile 0.15.7**. `main.ato` compiles to a
KiCad-native board + BOM. Edit and view with the **atopile VS Code extension**
(CLI and extension are both 0.15.7 — keep them in sync).

## Project layout — `well-bouy01/`
- `ato.yaml` — config (`requires-atopile: "^0.15.7"`, entry `main.ato:WellBouy`)
- `main.ato` — the design (edit this)
- `parts/` — LCSC-imported parts (ADS1115, JST-PH, KF301 screw terminal). The
  two connector parts are **hand-maintained** (named `p1/p2/p3` aliases added,
  `is_auto_generated` trait removed) — do not re-import them or you'll lose that.
- `layouts/default/default.kicad_pcb` — the generated board (open in KiCad 10)

## Commands (run inside `well-bouy01/`)
- `ato build` — compile → PCB + BOM
- `ato create part --search C<lcsc> -a` — import a JLCPCB part into `parts/`

## Editing / viewing in VS Code
Use the atopile extension for syntax, diagnostics, build, and the PCB/3D preview.
Note: freshly-built footprints are **stacked at the origin**, so the 3D preview
shows a pile until the board is laid out in KiCad. That's expected, not missing
parts.

## Design (`main.ato`) — 15 parts, all 1206 passives
Rails: `v3v3` (always-on) · `v5v_sw` (switched, load switch TBD) · `gnd`.
- **ADS1115** (U1, I²C 0x48): AIN0 = TDS (direct), AIN1 = turbidity (10k/6.8k),
  AIN2 = pH (10k/18k), AIN3 = spare
- I²C pull-ups 4.7k, ADS decoupling 100 nF
- **DS18B20** pull-up 4.7k → XIAO GPIO4
- **Connectors:** CN1/CN2 JST-PH (battery / solar), U2–U5 KF301 screw terminals
  (DS18B20 / turbidity / TDS / pH)

## Still to add (electrical)
- XIAO ESP32-S3 2×7 socket + wire nets to pins
- 5 V high-side load switch (→ `v5v_sw`) + reverse-polarity protection
- 4× M4 mounting holes

## Layout (KiCad 10 — after the electrical design is done)
atopile places parts at the origin; in KiCad you: spread them, draw the
**circular Edge.Cuts**, drop the **4× M4 holes** on the rim, route the ratsnest,
DRC → gerbers. Then the 3D preview shows the real round board.