# WellBouy data pipeline — TTN → Node-RED → InfluxDB 2.x → Grafana

Ingests the LoRaWAN uplinks (temperature, TDS, …) from The Things Network into
InfluxDB 2.x via Node-RED, and visualises them in Grafana.

```
TTN (MQTT, TLS 8883) ──► Node-RED (parse decoded_payload) ──► InfluxDB 2.x bucket ──► Grafana (Flux)
```

## 1. InfluxDB 2.x  (browser → :8086)
1. **Data → Buckets → Create bucket** → name **`wellbouy`** (retention as you like).
2. Note your **Org** name (bottom-left avatar → About, or the URL).
3. **Load Data → API Tokens → Generate API Token → Custom** → Write access to
   `wellbouy` (or "All Access" for now). **Copy the token** — shown once.

## 2. TTN MQTT credentials  (TTN Console → app → Integrations → MQTT)
- Host `eu1.cloud.thethings.network`, port **8883** (TLS)
- Username `<app-id>@ttn`
- Password = **Generate new API key** (copy it)

## 3. Node-RED  (browser → :1880)
1. **Menu → Manage palette → Install** `node-red-contrib-influxdb` (if not present).
2. **Menu → Import** → paste `node-red-flow.json` (this folder) → Import.
3. Open the **TTN uplinks** (mqtt-in) node → its broker `TTN EU1` → **Security tab**:
   username `<app-id>@ttn`, password = TTN API key. (TLS is already on.)
4. Open **write wellbouy** (influxdb out) → its server `InfluxDB 2.x`:
   - URL `http://localhost:8086` (or your host)
   - **Token** = the InfluxDB token from step 1
   - **Organization** = your org  ·  **Bucket** = `wellbouy`
   - Also set Organization/Bucket on the *out* node itself (replace `REPLACE_WITH_YOUR_ORG`).
5. **Deploy.** Watch the **point** debug node — on the next uplink you should see
   `[ {temperature_1: 26.3, luminosity_4: 327, rssi: -37, snr: 9}, {device: "..."} ]`.

The flow turns every **numeric** field in `uplink_message.decoded_payload` into an
Influx field (so new sensors appear automatically), plus `rssi`/`snr`, tagged by device.

## 4. Grafana  (browser → :3000)
1. **Connections → Data sources → Add → InfluxDB**
   - **Query language: Flux**
   - URL `http://localhost:8086` (or `http://influxdb:8086` if same Docker network)
   - Under InfluxDB Details: **Organization**, **Token**, **Default bucket** = `wellbouy`
   - Save & test.
2. New dashboard → add a panel, paste a Flux query:

**Temperature (°C):**
```flux
from(bucket: "wellbouy")
  |> range(start: v.timeRangeStart, stop: v.timeRangeStop)
  |> filter(fn: (r) => r._measurement == "wellbouy" and r._field == "temperature")
```
**TDS (ppm):**
```flux
from(bucket: "wellbouy")
  |> range(start: v.timeRangeStart, stop: v.timeRangeStop)
  |> filter(fn: (r) => r._measurement == "wellbouy" and r._field == "tds")
```
**Signal (RSSI):** same, `r._field == "rssi"`.

## Field names
The custom TTN formatter (`ttn-uplink-formatter.js`) renames the raw CayenneLPP
fields to semantic ones, so InfluxDB/Grafana see these:

| Field | Sensor | (raw LPP) |
|---|---|---|
| `temperature` | DS18B20 water temp (°C) | temperature_1 |
| `tds` | **TDS (ppm)** | luminosity_4 |
| `turbidity` | turbidity (NTU) — stage 04 | luminosity_3 |
| `ph` | pH — stage 05 | analog_in_2 |
| `rssi` / `snr` | LoRa link quality | — |

> TDS and turbidity ride on the **Luminosity** LPP type (0–65535), not Analog
> Input — Analog Input caps at ±327.67 and both values exceed that. pH fits
> Analog Input. The dashboard's water-quality panels are up top (live gauges →
> history); LoRa link quality is in the collapsed **Device & Connectivity** row.

## Downlinks — remote control (`node-red-downlink-flow.json`)

The node is **Class A**: it can only receive in the ~1 s RX window right after an
uplink, so a queued downlink is applied on its **next wake** (not instantly).
Commands (`main.cpp` → `handleDownlink`, opcodes in `include/config.h`):

| Command | Bytes | base64 | Effect |
|---|---|---|---|
| Set interval 5 min | `01 05` | `AQU=` | deep-sleep interval → 5 min |
| Set interval 30 min | `01 1E` | `AR4=` | deep-sleep interval → 30 min |
| Reboot | `02` | `Ag==` | soft-reboot the node |

Interval arg is **minutes** (1–255), floored at 60 s (`TX_INTERVAL_MIN_S`). The
live interval persists in RTC across deep sleep.

**Test it two ways:**
- **TTN Console** (quickest) → device → *Messaging → Downlink*: FPort **1**, payload
  bytes `01 05` (hex), *Push* → within one cycle the node prints
  `[DL] set interval -> 300 s` and its next `[SLEEP]` shows 300 s.
- **Node-RED**: *Import* `node-red-downlink-flow.json`, edit the **build TTN
  downlink** function (`APP` = your app id, confirm `DEV`), **Deploy**, then click
  an inject node. It publishes to `v3/<app>@ttn/devices/<dev>/down/push` over the
  same TTN broker as the uplink flow (the API key needs downlink write scope).