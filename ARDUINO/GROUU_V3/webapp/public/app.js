// ============================================================================
//  GROUU Well-Water Monitor — frontend logic.
//  Ported from the Claude Design DCLogic component; simulated data replaced by
//  the live /api/summary feed (temperature, tds, turbidity, ph, rssi, snr).
// ============================================================================
'use strict';

const ACCENT = '#65c298', AMBER = '#e0b050', RED = '#e07a5f';
const WATER = ['temperature', 'tds', 'turbidity', 'ph'];
const RANGES = ['6h', '24h', '7d'];

const META = {
  temperature: { label: 'Temperature', unit: '°C',  dec: 1 },
  tds:         { label: 'TDS',         unit: 'ppm', dec: 0 },
  turbidity:   { label: 'Turbidity',   unit: 'NTU', dec: 0 },
  ph:          { label: 'pH',          unit: '',    dec: 2 },
};
const RANGE_TICKS = {
  '6h':  ['-6h', '-4h', '-2h', 'now'],
  '24h': ['-24h', '-18h', '-12h', '-6h', 'now'],
  '7d':  ['-7d', '-5d', '-3d', '-1d', 'now'],
};

const state = { range: '24h', sel: 'turbidity', live: true, data: null };
let pollTimer = null;

// ── formatting / classification ────────────────────────────────────────────
function fmt(k, v) {
  if (v == null || Number.isNaN(v)) return '–';
  if (k === 'tds' || k === 'turbidity') return Math.round(v).toLocaleString('en-US');
  const dec = META[k] ? META[k].dec : 1;
  return Number(v).toFixed(dec);
}
function status(k, v) {
  const g = { color: ACCENT, bg: 'rgba(101,194,152,0.12)' };
  const a = { color: AMBER,  bg: 'rgba(224,176,80,0.14)' };
  const r = { color: RED,    bg: 'rgba(224,122,95,0.15)' };
  if (v == null) return { ...g, label: '—' };
  if (k === 'temperature') return { ...g, label: 'Normal' };
  if (k === 'tds')       return v < 600 ? { ...g, label: 'Good' } : v < 1000 ? { ...a, label: 'Elevated' } : { ...r, label: 'High' };
  if (k === 'turbidity') return v < 5   ? { ...g, label: 'Clear' } : v < 50  ? { ...a, label: 'Cloudy' }   : { ...r, label: 'Turbid' };
  if (k === 'ph')        return (v >= 6.5 && v <= 8.5) ? { ...g, label: 'Safe' }
                              : ((v >= 6.0 && v < 6.5) || (v > 8.5 && v <= 9)) ? { ...a, label: 'Acidic' }
                              : { ...r, label: 'Out of range' };
  return { ...g, label: '' };
}
function note(k) {
  return { temperature: 'Informational', tds: 'Drinking ideal < 600 ppm',
           turbidity: 'Drinking limit < 5 NTU', ph: 'Safe band 6.5 – 8.5' }[k] || '';
}
function relTime(iso) {
  const s = Math.floor((Date.now() - new Date(iso).getTime()) / 1000);
  if (!Number.isFinite(s) || s < 0) return '–';
  if (s < 60) return 'now';
  if (s < 3600) return Math.floor(s / 60) + ' min ago';
  if (s < 86400) return Math.floor(s / 3600) + ' h ago';
  return Math.floor(s / 86400) + ' d ago';
}

// ── SVG charts (area line + gradient) ───────────────────────────────────────
function pathParts(arr, w, h, pad) {
  const min = Math.min(...arr), max = Math.max(...arr), span = (max - min) || 1, n = arr.length;
  const x = (i) => (i / (n - 1)) * w;
  const y = (v) => h - pad - ((v - min) / span) * (h - pad * 2);
  let line = '';
  for (let i = 0; i < n; i++) line += (i ? 'L' : 'M') + x(i).toFixed(1) + ' ' + y(arr[i]).toFixed(1);
  return { line, area: line + `L${w} ${h}L0 ${h}Z`, lastX: x(n - 1), lastY: y(arr[n - 1]) };
}
function sparkSVG(arr, color, id) {
  if (!arr || arr.length < 2) return '';
  const W = 300, H = 46, p = pathParts(arr, W, H, 4);
  return `<svg viewBox="0 0 ${W} ${H}" preserveAspectRatio="none" style="width:100%;height:100%;display:block">
    <defs><linearGradient id="${id}" x1="0" y1="0" x2="0" y2="1">
      <stop offset="0%" stop-color="${color}" stop-opacity="0.28"/>
      <stop offset="100%" stop-color="${color}" stop-opacity="0"/></linearGradient></defs>
    <path d="${p.area}" fill="url(#${id})"/>
    <path d="${p.line}" fill="none" stroke="${color}" stroke-width="2" stroke-linejoin="round" stroke-linecap="round"/>
    <circle cx="${p.lastX.toFixed(1)}" cy="${p.lastY.toFixed(1)}" r="3" fill="${color}"/></svg>`;
}
function bigChartSVG(k, arr, color, range) {
  if (!arr || arr.length < 2)
    return `<div style="height:300px;display:grid;place-content:center;color:#6f756f;font-family:'IBM Plex Mono',monospace;font-size:14px;">Collecting data…</div>`;
  const W = 960, H = 300, padTop = 18, padBot = 34, padL = 6;
  const min = Math.min(...arr), max = Math.max(...arr), span = (max - min) || 1, n = arr.length;
  const x = (i) => padL + (i / (n - 1)) * (W - padL * 2);
  const y = (v) => H - padBot - ((v - min) / span) * (H - padTop - padBot);
  let line = '';
  for (let i = 0; i < n; i++) line += (i ? 'L' : 'M') + x(i).toFixed(1) + ' ' + y(arr[i]).toFixed(1);
  const area = line + `L${x(n - 1).toFixed(1)} ${H - padBot}L${x(0).toFixed(1)} ${H - padBot}Z`;
  const id = 'bg_' + k;
  let grid = '', ylabels = '';
  for (let g = 0; g <= 3; g++) {
    const gy = padTop + (g / 3) * (H - padTop - padBot), val = max - (g / 3) * span;
    grid += `<line x1="${padL}" y1="${gy.toFixed(1)}" x2="${W - padL}" y2="${gy.toFixed(1)}" stroke="#262b26" stroke-width="1"/>`;
    ylabels += `<text x="${W - padL - 2}" y="${(gy - 5).toFixed(1)}" text-anchor="end" font-family="IBM Plex Mono, monospace" font-size="13" fill="#6f756f">${fmt(k, val)}</text>`;
  }
  const ticks = RANGE_TICKS[range] || [];
  let xlabels = '';
  ticks.forEach((t, i) => {
    const xx = padL + (i / (ticks.length - 1)) * (W - padL * 2);
    const anchor = i === 0 ? 'start' : i === ticks.length - 1 ? 'end' : 'middle';
    xlabels += `<text x="${xx.toFixed(1)}" y="${H - 8}" text-anchor="${anchor}" font-family="IBM Plex Mono, monospace" font-size="13" fill="#6f756f">${t}</text>`;
  });
  return `<svg viewBox="0 0 ${W} ${H}" preserveAspectRatio="none" style="width:100%;height:300px;display:block">
    <defs><linearGradient id="${id}" x1="0" y1="0" x2="0" y2="1">
      <stop offset="0%" stop-color="${color}" stop-opacity="0.32"/>
      <stop offset="100%" stop-color="${color}" stop-opacity="0.02"/></linearGradient></defs>
    ${grid}
    <path d="${area}" fill="url(#${id})"/>
    <path d="${line}" fill="none" stroke="${color}" stroke-width="2.4" stroke-linejoin="round" stroke-linecap="round"/>
    <circle cx="${x(n - 1).toFixed(1)}" cy="${y(arr[n - 1]).toFixed(1)}" r="4" fill="${color}"/>
    <circle cx="${x(n - 1).toFixed(1)}" cy="${y(arr[n - 1]).toFixed(1)}" r="8" fill="${color}" opacity="0.22"/>
    ${ylabels}${xlabels}</svg>`;
}

// ── DOM ─────────────────────────────────────────────────────────────────────
const $ = (id) => document.getElementById(id);
const metricsEl = $('metrics'), tabsEl = $('tabs'), rangesEl = $('ranges'), bigChartEl = $('big-chart');

function metricCard(k, latest, vals) {
  const st = status(k, latest[k]);
  return `<div class="metric" style="border-top-color:${st.color}">
    <div class="metric-top">
      <span class="metric-label">${META[k].label}</span>
      <span class="chip" style="color:${st.color};background:${st.bg}">${st.label}</span>
    </div>
    <div class="metric-val"><span class="num">${fmt(k, latest[k])}</span><span class="unit">${META[k].unit}</span></div>
    <div class="spark">${sparkSVG(vals, st.color, 'sg_' + k)}</div>
    <div class="metric-note">${note(k)}</div>
  </div>`;
}

function render() {
  const d = state.data;
  if (!d) return;
  const { latest, series } = d;
  const vals = (k) => (series[k] || []).map((p) => p[1]);

  metricsEl.innerHTML = WATER.map((k) => metricCard(k, latest, vals(k))).join('');

  const selVals = vals(state.sel);
  const st = status(state.sel, latest[state.sel]);
  $('sel-name').textContent = META[state.sel].label;
  $('sel-unit').textContent = META[state.sel].unit || '—';
  bigChartEl.innerHTML = bigChartSVG(state.sel, selVals, st.color, state.range);
  if (selVals.length) {
    $('sel-min').textContent = fmt(state.sel, Math.min(...selVals));
    $('sel-max').textContent = fmt(state.sel, Math.max(...selVals));
    $('sel-avg').textContent = fmt(state.sel, selVals.reduce((a, b) => a + b, 0) / selVals.length);
  } else {
    $('sel-min').textContent = $('sel-avg').textContent = $('sel-max').textContent = '–';
  }

  tabsEl.querySelectorAll('.tab').forEach((b) => b.classList.toggle('on', b.dataset.k === state.sel));
  rangesEl.querySelectorAll('.rbtn').forEach((b) => b.classList.toggle('on', b.dataset.r === state.range));

  const rssi = latest.rssi, snr = latest.snr;
  $('rssi').textContent = rssi != null ? Math.round(rssi) : '–';
  $('snr').textContent = snr != null ? Number(snr).toFixed(1) : '–';
  $('link-quality').textContent = rssi == null ? '—' : rssi > -70 ? 'Excellent' : rssi > -100 ? 'Good' : 'Weak';
  $('rssi-chart').innerHTML = sparkSVG(vals('rssi'), ACCENT, 'sg_rssi');

  $('last-seen').textContent = d.updated ? relTime(d.updated) : '–';
  $('packets-rx').textContent = d.packets24h != null ? d.packets24h : '–';
  $('source-note').textContent = d.demo ? 'Demo data (no DB)' : 'Live';
}

async function fetchAndRender() {
  try {
    const r = await fetch('/api/summary?range=' + state.range, { cache: 'no-store' });
    if (!r.ok) throw new Error('HTTP ' + r.status);
    state.data = await r.json();
    render();
  } catch (e) {
    $('source-note').textContent = 'backend error';
    console.error('[summary]', e);
  }
}

function startPoll() {
  if (pollTimer) clearInterval(pollTimer);
  pollTimer = setInterval(() => { if (state.live) fetchAndRender(); }, 60000);
}

// ── init ─────────────────────────────────────────────────────────────────────
tabsEl.innerHTML = WATER.map((k) => `<button class="tab" data-k="${k}">${META[k].label}</button>`).join('');
tabsEl.querySelectorAll('.tab').forEach((b) => (b.onclick = () => { state.sel = b.dataset.k; render(); }));

rangesEl.innerHTML = RANGES.map((r) => `<button class="rbtn" data-r="${r}">${r}</button>`).join('');
rangesEl.querySelectorAll('.rbtn').forEach((b) => (b.onclick = () => { state.range = b.dataset.r; fetchAndRender(); }));

$('live-btn').onclick = () => {
  state.live = !state.live;
  $('live-label').textContent = state.live ? 'Live' : 'Paused';
  $('live-dot').style.background = state.live ? ACCENT : '#8a908a';
  $('live-btn').textContent = state.live ? 'Pause' : 'Resume';
  if (state.live) { fetchAndRender(); startPoll(); }
  else if (pollTimer) clearInterval(pollTimer);
};

setInterval(() => { $('clock').textContent = new Date().toLocaleTimeString('en-GB'); }, 1000);
$('clock').textContent = new Date().toLocaleTimeString('en-GB');

fetchAndRender();
startPoll();
