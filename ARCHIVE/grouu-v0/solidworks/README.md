# SOLIDESTUFA — Archive Audit & Reorganisation

Reconstruction of a legacy SolidWorks greenhouse project (EVOL, 2013–2014) recovered from
an external drive, audited without access to SolidWorks, and reorganised into a documented,
self-contained structure.

- **Source:** `/Volumes/EVOL2/SOLIDESTUFA/` (bulk-archived November 2017)
- **Audited:** July 2026
- **Tooling:** `swrefs.py` (audit), `swpack.py` (reorganisation)

---

## 1. Summary

The folder contained **1157 CAD files across 90 directories** with no manifest, no version
control, and broken cross-references. It held **two unrelated projects**: the greenhouse
(*estufa*, 2013–2014) and the ESYS/EXT modular profile and exhibition-stand system
(2008–2010), which the greenhouse consumes as a component library.

Six candidate "master" assemblies existed, differing by date prefix, initials suffix, and
Dropbox conflict markers. The audit identified the authoritative one, proved it resolves
completely, and established that later development exists only as unintegrated subsystems.

| Finding | Result |
|---|---|
| Authoritative master | `2013.09.28 Modelo Base de Estufa_AR.SLDASM` |
| Last saved (internal) | **2013-11-06 19:47** |
| Direct children | 73 (66 resolved dependencies after filtering) |
| Genuinely unresolved | **1** (`2013.11.06 pavimento para reforço_rl.sldasm`) |
| Superseded branch | `Modelo Base de Estufa_revPV_1` — abandoned 2013-08-27 |
| Later work, never merged | 5 subsystem assemblies (2014-03 → 2014-05) |

---

## 2. Method

SolidWorks `.sldasm`/`.sldprt` files are OLE2 compound documents. Every assembly stores the
paths of its referenced components internally. This is readable without SolidWorks:

1. Enumerate all streams in the compound file (`olefile`).
2. Decompress embedded zlib blocks (SolidWorks 2015+ compresses stream regions).
3. Regex for strings terminating in a SolidWorks extension, across Latin-1 and both
   UTF-16LE phase alignments.
4. Match by **basename**, not stored path — stored paths are absolute from machines that no
   longer exist.
5. Read `SummaryInformation` for internal save dates, author, last-saved-by.
6. Count `Config-N-*` storages to detect configuration-bearing parts.

Reference resolution then yields a directed graph: assemblies → components. Top-level
assemblies are nodes with no inbound edges; missing files are referenced names absent from
disk; orphans are files no assembly references.

### 2.1 Traps encountered

Four discoveries materially changed the results. Each is a general hazard for any legacy
CAD archive on macOS.

**AppleDouble pollution.** 1015 of 2030 files were `._` metadata stubs, created when macOS
writes to exFAT/NTFS. They inflated the file count by 100%, accounted for 300 of 456
apparent "top-level assemblies", 715 of 1124 orphans, and 118 duplicates. *Skip any file
matching `._*`.*

**Unicode normalisation mismatch.** macOS stores filenames in **NFD** (decomposed:
`c` + U+0327 combining cedilla); SolidWorks wrote references in **NFC** (precomposed `ç`).
Byte-different, visually identical. **89 files reported missing were present on disk** —
including `Reforço de águas` and `Reforço de Cumeeira`, core greenhouse components.
*Normalise both sides to NFC before comparing; retain raw paths for filesystem access, as
exFAT is not normalisation-insensitive.*

**Filesystem timestamps destroyed.** All 1015 mtimes fall on 2017-11-19/20/21 — the date the
archive was copied, not designed. Recency was unanswerable from the filesystem. The
`SummaryInformation` stream carries the true save date inside the file and survives copying.
*Never trust mtime on an archived project; read internal metadata.*

**Save-As lineage masquerading as hierarchy.** When an assembly is saved under a new name,
the previous filename persists in its streams. String-scanning cannot distinguish an
*ancestor* from a *child*, so version chains appear as containment. Mitigated by filtering
family-stem matches; the residue is why raw child counts are approximate.

### 2.2 Known limitations

- Reference extraction is a **superset**: stale references from deleted features and prior
  save locations are captured alongside live ones.
- The non-greedy regex emits both a full name and a truncated tail when a name contains an
  accented character (`reforço de águas.sldprt` *and* `guas.sldprt`). Fragments are
  filtered by pattern, not parsed.
- Configuration **names** require the `swXmlContents` stream (SolidWorks 2015+); pre-2015
  files yield counts only.
- Author fields are decoded Latin-1 and mojibake on non-ASCII names
  (`AndrÃ© Fangueiro` = André Fangueiro).

---

## 3. Findings

### 3.1 Composition

| | count |
|---|---|
| Real CAD files | 1157 (635 parts, 300 assemblies, 80 drawings) |
| Directories | 90 (63 containing a `SOLID/` segment) |
| Top-level assemblies | 155 |
| Duplicate basenames | 118 |
| AppleDouble stubs discarded | 1015 |

Save activity by year: **2009: 387 · 2010: 92 · 2011: 15 · 2012: 13 · 2013: 270 · 2014: 233**
— two distinct campaigns, matching the two projects.

Contributors (by files last saved): EVOL 429, André Fangueiro 276, Edgar Valente 122,
Rui Lopes 147, Pedro Viegas 7.

### 3.2 Configurations

**115 files carry more than one configuration**, peaking at **64 configs in `e1.x.sldprt`**
(`e1.x.sldasm`: 59; `ext1.x`: 45). The ESYS profile library is configuration-driven — one
part file holds every length and variant. This explains a large share of unresolved
references: discrete per-variant files were consolidated into configured parts and deleted,
while references to the old filenames persisted in assemblies never re-saved.

### 3.3 Missing files

Of 338 raw "missing" entries: 89 were the NFC/NFD artefact, 21 are Toolbox hardware
(resolving from the SolidWorks library, not the project), and the remainder are regex
fragments or genuinely absent.

**222 real missing files — of which only 6 are reachable from the greenhouse**, and 5 of
those are benign:

| file | status |
|---|---|
| `autorecover of canto simples.sldasm` | crash-recovery temp file, never a component |
| `e1 esys.sldprt` | superseded by `e.1 esys.sldprt` (present, in use) |
| `e1 esys + 2 x e1.ca.sldasm` | deleted variant; sibling `e1 esys + 2x e3.1` present |
| `ansi_inch_shcs0.sldprt` | Toolbox screw |
| `491123.sldasm` | wrapper only; parts `4911231/2/3.sldprt` all present |
| **`2013.11.06 pavimento para reforço_rl.sldasm`** | **the sole substantive gap** |

The remaining 216 belong exclusively to 2008–2010 ESYS/stand assemblies and are absent from
this drive entirely — likely resident in a separate ESYS project folder, of which
`A - ESYS/` and `D - EXT/` here appear to be partial copies.

### 3.4 Version resolution

Every candidate descends from one base model created **2013-03-26**. Internal save dates
resolve the branch structure:

| assembly | last saved | by |
|---|---|---|
| `modelo base de estufa` | 2013-06-04 | EVOL |
| `modelo base de estufa_revpv` | 2013-08-22 | Pedro Viegas |
| `modelo base de estufa_revpv_1` | **2013-08-27** | Pedro Viegas |
| `2013.08.28 modelo base de estufa_ar` | 2013-10-31 | EVOL |
| **`2013.09.28 modelo base de estufa_ar`** | **2013-11-06** | EVOL |

`_revPV` is *revisão Pedro Viegas* — the suffix is initials, not a feature. The branch was
abandoned on 2013-08-27; the `_AR` line continued for ten further weeks.

The two branches share 48 components. `_AR` uniquely carries the floor (`pavimento`) and
four discrete lexan supports at fixed lengths; `revPV_1` substitutes a generic
`suporte_lexan_fundo`/`_topo` pair plus two glazing profiles. Config counts confirm the
substitute supports hold **one configuration each** — `revPV_1` is not a consolidation of
the four, it simply carries less model.

### 3.5 The integration gap

Greenhouse development continued for six months **after** the master was last saved, but
never re-entered it. Five subsystem assemblies (roof hatch, rack, lateral vents, cabinet)
were built standalone under `Ventilação (caixilhos)/` and `Armário/`, and **no assembly
references them**.

The master is therefore the **November 2013 state**. Integrating the 2014 subsystems is
outstanding work that was never performed.

---

## 4. Resulting structure

```text
solidworks/
├── 2013.09.28 Modelo Base de Estufa_AR.SLDASM    # master — 2013-11-06
├── Dependencies/                                 # 66 components, flat
├── Detached Modules/                             # never merged into the master
│   ├── 2014.05.09 Conjunto Cremalheira/          #   9 deps  — rack
│   ├── 2014.05.05 Cremalheira/                   #   6 deps
│   ├── 2014.04.23 Conjunto Escotilha - circular/ #  12 deps  — roof hatch
│   ├── 2014.04.22 Conjunto Escotilha - simples/  #  10 deps
│   ├── 2014.03.24 Conjunto Lateral - ELIPSE pequena/  # 13 deps — lateral vent
│   ├── Estudos para Raised beds/                 #  64 deps  — 2013 study
│   └── Esteretoromia do Pavimento/               #   1 dep   — 2013 study
└── README.md
```

Each module carries its own `Dependencies/`. Excluded throughout: `backup of *`,
`autorecover of *`, Dropbox conflicted copies, `._*` stubs, and Toolbox hardware.

The last two entries under `Detached Modules/` are **2013 studies, not 2014 development** —
the classification test is structural (top-level, greenhouse, absent from the master), not
chronological.

### 4.1 Opening the master

SolidWorks resolves references against the assembly's **own folder**, not its subfolders.
Before first open:

> **Tools → Options → File Locations → Referenced Documents → Add…** → select `Dependencies/`

Alternatively, re-pack with `--flat` to place all components beside the master, which
resolves with no configuration.

### 4.2 Reproducing

```bash
python3 -m venv .venv && source .venv/bin/activate
pip install olefile

# audit -> graph, metadata, configs, interactive HTML report
python swrefs.py /Volumes/EVOL2/SOLIDESTUFA --json estufa.json --html estufa.html

# reorganise
python swpack.py estufa.json /Volumes/EVOL2/SOLIDESTUFA ./ESTUFA_FINAL --dry-run
python swpack.py estufa.json /Volumes/EVOL2/SOLIDESTUFA ./ESTUFA_FINAL
```

Useful flags: `--flat` (dependencies beside the master), `--keep-superseded` (preserve the
`revPV` dead branch), `--shallow` (skip zlib decompression).

> **Note:** the audit tools (`swrefs.py`, `swpack.py`) and the generated
> `_AUDIT_REPORT.txt` / `estufa.json` / `estufa.html` are not committed with this CAD set;
> the committed folder is the reorganised model tree only.

---

## 5. Recommendations for reuse

1. **Start from the master**, not the folder. It opens with one unresolved subassembly.
2. **Treat `A - ESYS/` and `D - EXT/` as a library**, not project content. They are
   configuration-driven and shared; 216 of their historical references are unrecoverable here.
3. **Integrating the 2014 subsystems is the first real design task** — it was never done.
4. **Recover `2013.11.06 pavimento para reforço_rl.sldasm`** if the floor reinforcement
   matters. Note `Pavimento/2013.11.06 Estrutura para Reforço-5 Travessas_RL.SLDASM` is
   present and may be its renamed successor.
5. **Do not re-derive versions from file dates.** The 2017 bulk copy destroyed them; use the
   internal save dates recorded in `estufa.json`.
