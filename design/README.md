# GROUU design system

The reference for building GROUU-branded UI. Extracted from the live docs site
(`docs/style.css` + the docs pages). Pair it with [`tokens.css`](tokens.css) —
import that and build with the `var(--grouu-*)` values.

Not a component library (there's no compiled React kit); it's the visual
language — palette, type, layout rhythm, and the component patterns the site
already uses. Reproduce these in whatever framework a future app uses.

---

## 1. Setup

```html
<link rel="preconnect" href="https://fonts.googleapis.com">
<link href="https://fonts.googleapis.com/css2?family=Space+Grotesk:wght@400;500;600;700&family=IBM+Plex+Mono:wght@400;500;600&display=swap" rel="stylesheet">
<link rel="stylesheet" href="tokens.css">
```

- **Space Grotesk** — everything (display + body).
- **IBM Plex Mono** — labels, captions, code, data/meta. Usually uppercase with letter-spacing.

---

## 2. Palette

Green on warm-neutral greens. One accent green, a near-black green ink, and three
surface tiers (white / light-green / dark). Don't introduce new hues; the only
non-green tints are semantic (water = blue, nutrient = warm green) and appear
only in schematics.

| Role | Token | Hex |
|---|---|---|
| Primary accent · links · CTAs | `--grouu-green` | `#008a60` |
| Link hover | `--grouu-green-hover` | `#00734f` |
| Green text on pale pills | `--grouu-green-deep` | `#0c6b4b` |
| Accent on dark · `::selection` | `--grouu-green-light` | `#65c298` |
| Pale-green pill / chip bg | `--grouu-green-pale` | `#e2f1e9` |
| Headings, strong text | `--grouu-ink` | `#16201a` |
| Body text | `--grouu-ink-body` | `#3a423a` |
| Secondary body | `--grouu-ink-muted` | `#4a524a` |
| Mono labels · captions · meta | `--grouu-ink-meta` | `#6b756b` |
| Text on dark | `--grouu-ink-on-dark` | `#eaf3ec` |
| Muted text on dark | `--grouu-ink-muted-on-dark` | `#bcccc0` |
| Page + light-green sections | `--grouu-bg` | `#e9f2ec` |
| White sections / cards | `--grouu-bg-white` | `#ffffff` |
| Subtle green surface | `--grouu-bg-tint` | `#f4f8f5` |
| Dark sections / code | `--grouu-bg-dark` | `#122018` |
| Deepest / dark borders | `--grouu-bg-darker` | `#0c150f` |
| Default border | `--grouu-line` | `#d5e4da` |
| Subtle divider | `--grouu-line-soft` | `#eef2ef` |
| Border on dark | `--grouu-line-dark` | `#274031` |

**Status colours** (dot · text · background):
- **Developed / live** → `#008a60` · `#0c6b4b` · `#e2f1e9`
- **Under development** → `#e0a020` · `#8a6d1f` · `#f5edd6`
- **Archived / n-a** → `#6b756b` · `#6b756b` · `#e6ede8`

---

## 3. Typography

Headings are heavy (600) and tight (negative tracking). Body is comfortable and
large. Mono is the "instrument panel" voice — uppercase, letter-spaced, for
labels, values, and captions.

| Style | Font | Size (desktop → mobile) | Weight | Tracking |
|---|---|---|---|---|
| Display / h1 | Space Grotesk | 66 → 36 px | 600 | −0.03em |
| Section / h2 | Space Grotesk | 38 → 26 px | 600 | −0.02em |
| Sub-heading / h3 | Space Grotesk | 21 px | 600 | — |
| Lead paragraph | Space Grotesk | 26 → 20 px | 400 | — |
| Body | Space Grotesk | 20 px | 400 | — |
| Small / meta | Space Grotesk | 15–16 px | 400 | — |
| Eyebrow (mono) | IBM Plex Mono | 15 px, UPPERCASE | 500 | +0.16em |
| Label / caption (mono) | IBM Plex Mono | 11–13 px | 500 | +0.04–0.1em |

Line-heights: headings ~1.1, body ~1.5.

---

## 4. Layout & rhythm

- **Content column:** `max-width: 1120px`, centred, `padding: 80px 40px` (→ `20px`
  sides on mobile). Sticky nav uses `1200px`.
- **Section rhythm:** stack full-width sections and **alternate the three surface
  tiers** so no two neighbours share a background:
  - white (`--grouu-bg-white`) — ink text
  - light-green (`--grouu-bg`) — ink text
  - dark (`--grouu-bg-dark`) — `--grouu-ink-on-dark` text; eyebrow/accents use `--grouu-green-light`
  Each section = full-bleed background + inner `max-width` column. Bottom border
  `1px solid var(--grouu-line)` between light sections.

---

## 5. Components / patterns

Concise specs; adapt to the target framework. All radii/colours via tokens.

**Nav** — sticky, translucent, blurred.
```css
position:sticky; top:0; background:rgba(233,242,236,0.82); backdrop-filter:blur(12px);
border-bottom:1px solid var(--grouu-line);
```
Links are mono, uppercase, `letter-spacing:.04em`. A single primary link is a green pill (below).

**Section header** — mono eyebrow → heading → lead.
```html
<div class="eyebrow">Section label</div>   <!-- mono, UPPERCASE, +0.16em, color:var(--grouu-green) -->
<h2>Heading</h2>                            <!-- 38px/600/−0.02em, color:var(--grouu-ink) -->
<p class="lead">One factual sentence.</p>   <!-- 20px, color:var(--grouu-ink-muted) -->
```
On dark sections the eyebrow uses `--grouu-green-light` and the lead `--grouu-ink-muted-on-dark`.

**Card** — white, hairline border, `14–16px` radius.
```css
background:var(--grouu-bg-white); border:1px solid var(--grouu-line);
border-radius:var(--grouu-r-md); padding:26px 28px;
```
Hover accent: `border-color:var(--grouu-green)` (transition 0.15s). Feature cards invert to `--grouu-bg-dark`.

**Button / link pill.**
```css
/* primary */  background:var(--grouu-green); color:#f6f5f1; border-radius:var(--grouu-r-pill); padding:13px 26px;
/* pale     */  background:var(--grouu-green-pale); color:var(--grouu-green); border:1px solid #bfe0cd; padding:9px 16px;
```
Both mono, often with a `↗` (external) or `→` (internal) suffix.

**Pill / badge + status.** 100px radius, mono, small. A round 9px dot + label:
```html
<span class="dot ok"></span> <span class="badge-txt ok">Developed · live</span>
```
- ok: dot `#008a60` (with `box-shadow:0 0 0 4px rgba(0,138,96,.16)`), text `#0c6b4b`, bg `#e2f1e9`
- warn: dot `#e0a020`, text `#8a6d1f`, bg `#f5edd6`
- archived: text/dot `#6b756b`, bg `#e6ede8`, border `#cdd8d0`

**Table.** Mono uppercase header on tint; hairline rows; wrap in `overflow-x:auto`.
```css
th{ font-family:var(--grouu-font-mono); text-transform:uppercase; font-size:12px; letter-spacing:.05em;
    color:var(--grouu-ink-meta); background:var(--grouu-bg-tint); border-bottom:1px solid var(--grouu-line); }
td{ border-bottom:1px solid var(--grouu-line-soft); }
```

**Code / data block.** Dark, mono, horizontally scrollable.
```css
background:var(--grouu-bg-dark); border:1px solid var(--grouu-line-dark);
border-radius:var(--grouu-r-sm); color:#cfe6d8; font-family:var(--grouu-font-mono);
font-size:13px; line-height:1.6; padding:20px 22px; overflow-x:auto;
```

**Schematic boxes** (block diagrams). White box + `1.5px solid var(--grouu-ink)` border,
`8px` radius; a mono uppercase kicker (`--grouu-green`), a 600 name, a mono sub-line
(`--grouu-ink-meta`). Variants: `dark` (filled `--grouu-bg-dark`), `pale` (`--grouu-green-pale`),
`dashed` accent border, filled-green nodes (`--grouu-green`, `--grouu-ink-on-dark` text).
Arrows `→` / `▼` in `--grouu-ink`; on dark, connectors/lines in `--grouu-green-light`.

**Image placeholder** — dashed, diagonal-hatch, centred caption:
```css
background:repeating-linear-gradient(45deg,#dcece2,#dcece2 12px,#d5e6dc 12px,#d5e6dc 24px);
border:1.5px dashed #9dbfaa; color:#5c6b60;
```

---

## 6. Voice

Factual and enumerated. State **what a thing is**, list its parts, and say plainly
what is / isn't done ("Built: … Not built: …"). No marketing adjectives, no
"seamless/powerful/revolutionary", no calls to excitement. Mono is for the facts
(values, part numbers, statuses); sans is for the plain-language sentence.

---

## 7. Responsive

- Multi-column grids step 4 → 2 (≤980px) → 1 (≤760px).
- Two-column layouts collapse to one at ≤760px.
- Horizontal flow diagrams stack to a column and their `→` arrows rotate to `▼`.
- Section padding tightens to `20px` sides; hero/heading fonts scale down (see the size table).
- Wide content (tables, code, fixed-size diagrams) scrolls inside its own
  `overflow-x:auto` container — the page never scrolls sideways.
