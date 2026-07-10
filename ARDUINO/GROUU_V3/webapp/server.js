// ============================================================================
//  WellBouy dashboard — tiny Express backend that serves the static frontend
//  and exposes the well-water data from InfluxDB as JSON.
//
//  Endpoints:
//    GET /health              -> { ok, demo }
//    GET /api/summary?range=  -> { device, updated, latest{...}, series{...} }
//    range ∈ 6h | 24h | 7d | 30d  (default 24h)
//
//  Config via env (see .env.example). With no INFLUX_TOKEN (or DEMO=1) it runs
//  in DEMO mode serving synthetic data, so the frontend can be developed and
//  previewed without a database.
// ============================================================================
import express from 'express';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { InfluxDB } from '@influxdata/influxdb-client';

const __dirname = path.dirname(fileURLToPath(import.meta.url));

const {
  PORT = 3000,
  INFLUX_URL,
  INFLUX_TOKEN,
  INFLUX_ORG,
  INFLUX_BUCKET = 'wellbouy',
  INFLUX_MEASUREMENT = 'wellbouy',
  DEMO,
} = process.env;

const DEMO_MODE = DEMO === '1' || !INFLUX_TOKEN;
const LATEST_FIELDS  = ['temperature', 'tds', 'turbidity', 'ph', 'rssi', 'snr'];
const HISTORY_FIELDS = ['temperature', 'tds', 'turbidity', 'ph', 'rssi'];

// range -> flux window duration + aggregate bucket + span in ms (for demo)
const RANGES = {
  '6h':  { dur: '-6h',  win: '2m',  spanMs: 6  * 3600e3,  stepMs: 120e3 },
  '24h': { dur: '-24h', win: '10m', spanMs: 24 * 3600e3,  stepMs: 600e3 },
  '7d':  { dur: '-7d',  win: '1h',  spanMs: 7  * 86400e3, stepMs: 3600e3 },
  '30d': { dur: '-30d', win: '6h',  spanMs: 30 * 86400e3, stepMs: 21600e3 },
};

// ---------------------------------------------------------------------------
//  InfluxDB
// ---------------------------------------------------------------------------
const queryApi = DEMO_MODE
  ? null
  : new InfluxDB({ url: INFLUX_URL, token: INFLUX_TOKEN }).getQueryApi(INFLUX_ORG);

const fieldSet = (fields) => '[' + fields.map((f) => `"${f}"`).join(', ') + ']';

async function influxLatest() {
  const flux = `
    from(bucket: "${INFLUX_BUCKET}")
      |> range(start: -30d)
      |> filter(fn: (r) => r._measurement == "${INFLUX_MEASUREMENT}" and contains(value: r._field, set: ${fieldSet(LATEST_FIELDS)}))
      |> last()`;
  const rows = await queryApi.collectRows(flux);
  const latest = {};
  let updated = null;
  for (const r of rows) {
    latest[r._field] = r._value;
    if (!updated || r._time > updated) updated = r._time;
  }
  return { latest, updated };
}

async function influxSeries(range) {
  const { dur, win } = RANGES[range] || RANGES['24h'];
  const flux = `
    from(bucket: "${INFLUX_BUCKET}")
      |> range(start: ${dur})
      |> filter(fn: (r) => r._measurement == "${INFLUX_MEASUREMENT}" and contains(value: r._field, set: ${fieldSet(HISTORY_FIELDS)}))
      |> aggregateWindow(every: ${win}, fn: mean, createEmpty: false)`;
  const rows = await queryApi.collectRows(flux);
  const series = Object.fromEntries(HISTORY_FIELDS.map((f) => [f, []]));
  for (const r of rows) {
    if (r._value == null || !series[r._field]) continue;
    series[r._field].push([new Date(r._time).getTime(), r._value]);
  }
  for (const f of HISTORY_FIELDS) series[f].sort((a, b) => a[0] - b[0]);
  return series;
}

async function influxCount24h() {
  const flux = `
    from(bucket: "${INFLUX_BUCKET}")
      |> range(start: -24h)
      |> filter(fn: (r) => r._measurement == "${INFLUX_MEASUREMENT}" and r._field == "temperature")
      |> count()`;
  const rows = await queryApi.collectRows(flux);
  return rows.length ? Number(rows[0]._value) : 0;
}

// ---------------------------------------------------------------------------
//  Demo data (no DB): realistic near-clear well water, gently varying
// ---------------------------------------------------------------------------
const round = (v, d = 0) => { const m = 10 ** d; return Math.round(v * m) / m; };
const noise = (a) => (Math.random() - 0.5) * 2 * a;

function demoSeries(range) {
  const { spanMs, stepMs } = RANGES[range] || RANGES['24h'];
  const now = Date.now();
  const n = Math.floor(spanMs / stepMs);
  const series = { temperature: [], tds: [], turbidity: [], ph: [], rssi: [] };
  for (let i = n; i >= 0; i--) {
    const t = now - i * stepMs;
    const h = t / 3600e3; // hours, for slow diurnal drift
    series.temperature.push([t, round(17.5 + 2.2 * Math.sin(h / 4) + noise(0.25), 1)]);
    series.tds.push([t, Math.round(430 + 60 * Math.sin(h / 6) + noise(18))]);
    series.turbidity.push([t, Math.max(0, round(6 + 4 * Math.sin(h / 3) + noise(2), 1))]);
    series.ph.push([t, round(7.35 + 0.22 * Math.sin(h / 5) + noise(0.05), 2)]);
    series.rssi.push([t, Math.round(-47 + 5 * Math.sin(h / 2) + noise(3))]);
  }
  return series;
}

function demoSummary(range) {
  const series = demoSeries(range);
  const last = (f) => series[f][series[f].length - 1][1];
  return {
    device: 'grouu-bouy-proto',
    updated: new Date().toISOString(),
    demo: true,
    range,
    latest: {
      temperature: last('temperature'),
      tds: last('tds'),
      turbidity: last('turbidity'),
      ph: last('ph'),
      rssi: last('rssi'),
      snr: round(9 + noise(1), 1),
    },
    series,
    packets24h: 46 + Math.round(noise(2)),
  };
}

// ---------------------------------------------------------------------------
//  App
// ---------------------------------------------------------------------------
const app = express();
app.disable('x-powered-by');
app.use(express.static(path.join(__dirname, 'public'), { maxAge: '1h' }));

app.get('/health', (_req, res) => res.json({ ok: true, demo: DEMO_MODE }));

app.get('/api/summary', async (req, res) => {
  const range = RANGES[req.query.range] ? req.query.range : '24h';
  try {
    if (DEMO_MODE) return res.json(demoSummary(range));
    const [{ latest, updated }, series, packets24h] = await Promise.all([
      influxLatest(),
      influxSeries(range),
      influxCount24h(),
    ]);
    res.json({ device: req.query.device || null, updated, demo: false, range, latest, series, packets24h });
  } catch (err) {
    console.error('[api/summary]', err.message || err);
    res.status(502).json({ error: 'influx_query_failed', message: String(err.message || err) });
  }
});

app.listen(PORT, () => {
  console.log(`WellBouy dashboard on :${PORT}  (${DEMO_MODE ? 'DEMO data' : `InfluxDB ${INFLUX_URL} / ${INFLUX_BUCKET}`})`);
});
