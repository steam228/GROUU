# GROUU V0 (GrouuPro) — archive manifest

The complete GrouuPro archive is **1.5 GB / ~1650 files**. Most of it is heavy
binary source (3D/CAD, raw photos, Keynote) that does not belong in git, so this
repo commits only the light IP and **catalogues the rest**. The full originals
live on the maintainer's OneDrive backup:

```
OneDrive/…/BACKUPCLOUDEVOL/LEVOWORKS/02 - PRODUÇÕES/06 - ASFERT GROUU PRO
```

A complete file-by-file listing with sizes is in
[`full-file-index.txt`](full-file-index.txt).

## Top-level folders

| Folder | Size | Files | What it is | In git? |
|---|---|---|---|---|
| `3D` | 699 M | 46 | 3D models & renders (SketchUp etc.) | backup only |
| `SOLID` | 175 M | 417 | SolidWorks parts / assemblies / drawings | backup only |
| `CAD` | 15 M | 99 | AutoCAD 2D drawings (`.dwg`) | backup only |
| `RHINO` | 27 M | 10 | Rhino 3D models | backup only |
| `DP` | 253 M | 141 | Project development — construction photos, wiring schematic, task lists, pump/valve specs | schematic committed; photos backup only |
| `FOTO` | 214 M | 16 | Large product / build photographs | backup only |
| `KEY` | 102 M | 1 | Keynote presentation | backup only |
| `GR` | 6.2 M | 120 | Graphics / UI assets (Flat-UI) | backup only |
| `OR` | 6.6 M | 38 | Original / misc source material | backup only |
| `SEND` | 2.3 M | 38 | Deliverables sent | backup only |
| `CODE` | 28 M | 688 | `Grow-git` self-host dashboard experiment | purged (not the deployed system) |
| `BOARDS` | 2.4 M | 22 | `hex-motor-controller` EAGLE PCB | excluded — unrelated to GrouuPro |
| top-level | — | — | Presentation (`.ai`/`.pdf`), structure/connector PNGs, base-model PDFs | **key ones → `drawings/`** |

## Committed to this repo

- **Firmware** — the real GrouuPro code lives at
  [`ARCHIVE/arduino/GROUU_GREENHOUSE_0/`](../arduino/GROUU_GREENHOUSE_0) (Yún +
  probe + Temboo sketches, servo/pump actuation). Credentials redacted.
- **`ARCHIVE/grouu-v0/drawings/`** — EXP renders, connector & structure details,
  `apresentação 1.pdf`, base-model PDFs, and the GrouuPro fertigation schematic.
- `README.md`, this manifest, `full-file-index.txt`, and `photos/` (placeholders).

## Deliberately excluded

- **`Grow-git`** (a self-hosted Node/Angular dashboard experiment) — *not* the
  deployed system (Temboo → Google Sheets was), and of no consequence to V0.
  Purged; it survives only on the backup.
- **The `hex-motor-controller` EAGLE PCB** — unrelated to GrouuPro. No custom
  board was ever designed; pumps/motors ran on perfboard MOSFETs.
- **Third-party supplier brochures** (Lexan Thermoclear, BWF profiles) — kept on
  backup, not committed (their copyright).
- **Heavy CAD / 3D / raw photos / Keynote** — backup only, catalogued above.
