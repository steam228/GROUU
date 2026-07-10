# GROUU Well-Water Monitor — web dashboard

Public, read-only dashboard for the WellBouy node. Tiny **Express** backend
queries **InfluxDB** and serves a static frontend (the GROUU "Well-Water
Dashboard" design, ported to plain HTML/JS — no build step, no framework).

```
InfluxDB (wellbouy bucket) ──► Express /api/summary ──► static dashboard
                                                        ▲ Cloudflare tunnel (subdomain)
```

## Preview it (demo data, no database)
Runs with synthetic data if no `INFLUX_TOKEN` is set — good for checking the look.

```bash
# with Docker (recommended):
cd webapp && docker compose up --build      # then http://localhost:3000
#   (uncomment the ports: mapping in docker-compose.yml first)

# or with Node directly:
cd webapp && npm install && DEMO=1 npm start # then http://localhost:3000
```

## Wire it to real data
```bash
cp .env.example .env      # fill INFLUX_URL / INFLUX_TOKEN / INFLUX_ORG
```
- `INFLUX_TOKEN` = an InfluxDB **read** token for the `wellbouy` bucket.
- `INFLUX_URL` = `http://influxdb:8086` if this container shares a Docker network
  with InfluxDB, otherwise the host/IP.

Endpoints: `GET /health`, `GET /api/summary?range=6h|24h|7d` →
`{ device, updated, latest{temperature,tds,turbidity,ph,rssi,snr}, series{…}, packets24h }`.

**Real vs. simulated:** temperature / TDS / turbidity / pH / RSSI / SNR, last-seen
and interval are **live**. Battery % and packet-loss are shown as `—/pending`
(the node doesn't send battery telemetry or sequence numbers yet).

## Deploy on the Pi (Docker + Cloudflare tunnel, subdomain)
1. Put this container on the **same Docker network** as your Cloudflare tunnel /
   reverse proxy so the tunnel can reach it by name. Edit `docker-compose.yml`'s
   `networks:` to match your existing `external` network, then:
   ```bash
   docker compose up -d --build
   ```
2. Point a **subdomain** at it. Two common setups:

   **A. cloudflared with a `config.yml`** — add an ingress rule:
   ```yaml
   ingress:
     - hostname: well.YOURDOMAIN.com
       service: http://wellbouy-dashboard:3000   # container name : port
     # …existing rules…
     - service: http_status:404
   ```
   then `cloudflared tunnel route dns <tunnel> well.YOURDOMAIN.com` and restart cloudflared.

   **B. Zero-Trust dashboard-managed tunnel** — Networks → Tunnels → your tunnel →
   Public Hostname → **Add**: subdomain `well`, domain `YOURDOMAIN.com`,
   service `http://wellbouy-dashboard:3000`.

That's it — `https://well.YOURDOMAIN.com` serves the dashboard, isolated from the
app already on the root domain.

> Share your existing `docker-compose.yml` / cloudflared config and I'll fill in
> the exact network name + ingress rule so it drops straight in.
