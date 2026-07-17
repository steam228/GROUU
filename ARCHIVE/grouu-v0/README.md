# GROUU V0 — GrouuPro (centralized greenhouse)

**Status: archived.** Circa 2013–2014. Not in active development — kept as the
origin of the GROUU lineage.

GrouuPro was a fully-built, full-stack IoT greenhouse: a physical structure, a
custom control board, an Arduino front-end, and a real-time web dashboard. It is
the direct ancestor of everything GROUU became — the same **sensor → server →
storage → dashboard** architecture that V2/V3 rebuild with modern tooling.

- **Sole author:** the GROUU maintainer ([github.com/steam228](https://github.com/steam228)).
- **Build assistance:** Rui Lopes.

## Architecture (2013 tech)

```
Arduino (analog sensors, serial "A0:value" @9600)
        │  USB serial
   Node.js on a Raspberry Pi
     Express 3 · serialport · socket.io · Mongoose → MongoDB · Passport auth
        │  socket.io (live)
   Angular.js + d3 / rickshaw dashboard
```

The same shape as the current stack — only the parts changed:

| GrouuPro (V0, 2013) | GROUU Stack (V2/V3) |
|---|---|
| Arduino → serial | ESP32 / XIAO → WiFi/LoRaWAN → MQTT |
| Node.js + socket.io | Mosquitto + Node-RED |
| MongoDB (Mongoose) | InfluxDB |
| Angular + d3 / rickshaw | Grafana + web dashboard |

The web app (`code/grow/`) already modelled the greenhouse as data:
`temp`, `humidity`, `moisture`, `light`, `leaf`, and `ensaio` (trial/experiment),
with dashboard views for **environment, watering, reports, journal, probe and
trials**. The Arduino sketch itself was an early serial streamer; the depth was
in the server and dashboard.

## Hardware

- **Structure:** LEXAN 8 mm polycarbonate glazing on BWF aluminium profiles.
- **Water:** SHURFLO pump + solenoid valves.
- **Control board:** a custom EAGLE-designed PCB — the *hex motor controller*
  (`hardware/hex-motor-controller/`), driving the motors/valves.

## What's in this archive

Committed here (light, ~2 MB — the actual IP):

- `code/grow/` — the Grow web app + `Arduino/Arduino.ino`, with the OneDrive-mangled
  git folder and vendored front-end libraries (jQuery/Angular/d3/…) stripped;
  `package.json` / `bower.json` still declare them.
- `hardware/hex-motor-controller/` — EAGLE schematic, board, gerbers, render, PDF.
- `drawings/` — structure & connector detail drawings, the project presentation
  (`apresentação 1.pdf`), base models, and the GrouuPro wiring schematic.

**Not committed** (heavy binary source — 3D/CAD, raw photos, Keynote; ~1.5 GB):
these live on the maintainer's backup. Everything is catalogued in
[`MANIFEST.md`](MANIFEST.md) and indexed file-by-file in
[`full-file-index.txt`](full-file-index.txt), so nothing is lost — the originals
are just referenced, not stored in git.

## Licence

Part of GROUU — GPLv3 for code, CC-BY-SA 4.0 for hardware & docs (see the repo
root `LICENSE.txt`). Third-party material (Lexan/BWF supplier brochures, vendored
libraries) is intentionally excluded and retains its own licensing.
