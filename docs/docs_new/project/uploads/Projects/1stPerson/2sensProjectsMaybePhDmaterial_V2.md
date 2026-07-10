# Environmental Sensing Projects — Hardware Reference (V3)

> Three 1st-person design projects sharing a common XIAO + Wio-SX1262 hardware base. Each targets a different environment the designer inhabits: well water (B1), open water (B2), urban air (A).

> **Changelog V3:** B1 (well) locked to **LoRaWAN → TTN, no GPS, no solar** as the semester deliverable. Added verified Wio-SX1262 pin map + RadioLib version note, a Meshtastic analog-sensor architecture caveat (B2/A), and a stack mechanical-fit warning. B2 + A unchanged structurally.

**Coursework plan:** [[../../Research/Courses/Media_Tech_and_Tech_Lab_Coursework_2026|Media Tech & Tech Lab Coursework]] **Task breakdown:** [[../../../inboxArchive/buoy-project-tasks-scheduling|Buoy Project Tasks]] **B1 firmware kickstart:** [[B1_Well_Sensor_Kickstart|B1 Well Sensor Kickstart]]

## Index

- [[#Shared Hardware Base]]
- [[#Project B1 — Well Water Quality Sensor]]
- [[#Project B2 — Open Water Buoy]]
- [[#Project A — Air Quality Helmet]]
- [[#Communication]]
- [[#Fabrication Notes]]
- [[#Shopping Lists]]
- [[#Next Steps]]

---

## Shared Hardware Base

All three projects use the Seeed XIAO form factor with the Wio-SX1262 LoRa radio shield.

|Component|Notes|
|---|---|
|**Seeed Wio-SX1262**|LoRa shield, XIAO form factor, B2B/SPI|
|**XIAO ESP32-S3**|WiFi + BLE, more community examples, higher power draw — [Seeed](https://www.seeedstudio.com/Wio-SX1262-with-XIAO-ESP32S3-p-5982.html)|
|**XIAO nRF52840**|BLE 5.0 only, lower power (~5–15 mA active vs ~100 mA), better ADC — [Seeed](https://www.seeedstudio.com/XIAO-nRF52840-Wio-SX1262-Kit-for-Meshtastic-p-6400.html)|
|**Grove Shield for XIAO**|Replaces custom carrier PCB; exposes Grove connectors (analog, I2C, UART, digital) — [Seeed](https://www.seeedstudio.com/Grove-Shield-for-Seeeduino-XIAO-p-4621.html)|
|**5V boost converters**|In stock|

> **Pin map (Wio-SX1262 over B2B, RadioLib):** `Module(41, 39, 42, 40)` = NSS 41, DIO1 39, RST 42, BUSY 40; SCK/MISO/MOSI on the default hardware SPI bus. **Pin RadioLib to exactly 6.6.0** — Seeed's example fails to compile on newer versions.

### Batteries

|Use case|Recommended|Link|
|---|---|---|
|Well sensor / buoy (capacity)|LiPo 1S 4500 mAh JST 2-pin|[Mauser 035-5302](https://mauser.pt/035-5302/bateria-3-7v-4500mah-li-po-34x97x12mm) (34x97x12 mm, PCB protection)|
|Helmet (compact)|LiPo 1S 2000 mAh JST-PH 2.0|[Adafruit #2011](https://www.adafruit.com/product/2011)|
|Dev/test (XIAO native)|300 mAh Seeed XIAO pack|[Seeed 110991440](https://www.seeedstudio.com/Battery-for-Seeed-Studio-XIAO-p-5252.html)|

> Verify JST polarity before connecting — Seeed and Adafruit conventions occasionally differ.

---

## Project B1 — Well Water Quality Sensor

> **Semester assignment basis (Tech Lab).** Simpler, quicker to build. Waterproof hull, **no GPS** (fixed location — coordinates hardcoded in payload), no solar. Freshwater sensors only. **Comms: LoRaWAN** (OTAA, EU 868 MHz) → TTN → MQTT bridge → RPi server stack. See [[Communication#LoRaWAN (B1)|below]]. **Firmware quickstart:** [[B1_Well_Sensor_Kickstart|B1 Well Sensor Kickstart]]

### Board stack

```
XIAO MCU         <- top
Grove Shield     <- middle (sensors via Grove cables)
Wio-SX1262       <- bottom (SPI via B2B connector)
```

> ⚠️ **Mechanical check:** the Grove Shield (XIAO edge pins) and the Wio-SX1262 (B2B, bottom) occupy the same vertical space. Dry test-fit the three-board stack before designing the hull — the shield may foul the B2B mate.

### BOM

| Component                                                                                        | Interface                              | VCC      | Library / Docs                                                                                                                                 | Calibration & Notes                                                                                                                      | Status               |
| ------------------------------------------------------------------------------------------------ | -------------------------------------- | -------- | ---------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------- | -------------------- |
| [Grove TDS](https://www.seeedstudio.com/Grove-TDS-Sensor-p-4400.html)                            | Analog (Grove)                         | 3.3/5V   | [DFRobot GravityTDS](https://github.com/DFRobot/GravityTDS)                                                                                    | **Two-point cal:** 84 + 1413 uS/cm KCl. Adjust `kValue`. Temp compensation required — use DS18B20. Output ~0–2.3V (safe for 3.3V ADC).   | **In stock**         |
| [SEN0189](https://wiki.dfrobot.com/Turbidity_sensor_SKU__SEN0189) Turbidity                      | Analog                                 | 5V       | [DFRobot wiki](https://wiki.dfrobot.com/Turbidity_sensor_SKU__SEN0189)                                                                         | Factory calibrated. Verify zero in clean distilled water. Clean optical window at deployment. **Output up to 4.5V — divider mandatory.** | **In stock — check** |
| [DS18B20 Waterproof](https://www.adafruit.com/product/381) Temp                                  | 1-Wire (digital Grove pin)             | 3.3V     | [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library) + [OneWire](https://github.com/PaulStoffregen/OneWire) | Factory calibrated +/-0.5 C. 4.7 kOhm pullup on data line.                                                                               | **In stock**         |
| [Phidgets ASP200-2-1M-BNC](https://www.phidgets.com/?prodid=412) pH Probe                        | BNC -> Phidgets 1130 -> Analog (Grove) | --       | --                                                                                                                                             | Gel-filled sealed electrode. Range 0-14 pH. No maintenance.                                                                              | **In stock**         |
| [Phidgets 1130](https://www.phidgets.com/?prodid=103) pH/ORP Adapter                             | Analog -> Grove analog pin             | 4.5-5.3V | [Arduino forum example](https://forum.arduino.cc/t/phidgets-1130-ph-adapter/22638)                                                             | BNC -> +/-400 mV analog out. Two-point cal (pH 4.0 + 7.0). <3 mA draw. **Verify output range vs 3.3V ADC; add divider if needed.**       | **In stock**         |
| [Grove Shield for XIAO](https://www.seeedstudio.com/Grove-Shield-for-Seeeduino-XIAO-p-4621.html) | --                                     | --       | --                                                                                                                                             | Replaces custom carrier PCB. Routes Grove connectors to XIAO GPIO.                                                                       | **TO ORDER**         |
| Voltage divider 10 kOhm / 6.8 kOhm                                                               | --                                     | --       | --                                                                                                                                             | **Mandatory** — SEN0189 outputs up to 4.5V, ADC max 3.3V                                                                                 | **In stock**         |
| M12 waterproof cable glands                                                                      | --                                     | --       | --                                                                                                                                             | Sensor wire pass-throughs for sealed hull.                                                                                               | **TO ORDER**         |
| BNC panel-mount bulkhead connector                                                               | --                                     | --       | --                                                                                                                                             | Waterproof pass-through for pH probe cable.                                                                                              | **TO ORDER**         |
| O-rings (size TBD after hull design)                                                             | --                                     | --       | --                                                                                                                                             | Hull seal. Size depends on enclosure.                                                                                                    | **TO ORDER**         |
| LiPo 1S 4500 mAh                                                                                 | --                                     | --       | --                                                                                                                                             | Mains-rechargeable (no solar). Long autonomy for well deployment.                                                                        | **TO ORDER**         |
| pH calibration buffers (4.0 + 7.0)                                                               | --                                     | --       | --                                                                                                                                             | Two-point calibration.                                                                                                                   | **TO ORDER**         |
| TDS calibration KCl (84 + 1413 uS/cm)                                                            | --                                     | --       | --                                                                                                                                             | Two-point calibration.                                                                                                                   | **TO ORDER**         |

### Grove Shield wiring summary

|Sensor|Grove port type|Notes|
|---|---|---|
|Grove TDS|Analog|Direct Grove cable|
|SEN0189 Turbidity|Analog|Via voltage divider board before Grove connector|
|DS18B20|Digital|4.7 kOhm pullup on signal line|
|Phidgets 1130 (pH)|Analog|Wire 3-pin Dupont output to Grove analog pin|

### ADC note

Sample TDS, turbidity, and pH **sequentially with >50 ms delay** — simultaneous ADC reads cause crosstalk on ESP32. The ESP32-S3 internal ADC is noisy/nonlinear; for the precision-sensitive pH channel, consider an **external I2C 16-bit ADC (ADS1115)** rather than the internal ADC. On nRF52840 the ADC is cleaner but sequential sampling is still recommended.

### Firmware (B1)

- PlatformIO, board `seeed_xiao_esp32s3`, framework `arduino`
- **RadioLib pinned to 6.6.0**
- LoRaWAN OTAA join to TTN, EU868; payload encoded as Cayenne LPP
- Fixed lat/lon hardcoded as constants in the payload (no GPS)
- ESP32 deep sleep between transmit cycles; interval as a single `#define` (default 600 s)
- Full kickstart prompt + connection table: [[B1_Well_Sensor_Kickstart|B1 Well Sensor Kickstart]]

---

## Project B2 — Open Water Buoy

> **Post-semester. Directly saltwater-ready.** Adds GPS, solar power, and saltwater-rated sensors over B1. Keeps Phidgets pH probes (gel-filled — will test with automated sample collection strategy before switching to Atlas). **Comms: Meshtastic / MeshCore** mesh network. See [[Communication#Meshtastic / MeshCore (B2, A)|below]].

### Board stack

```
L76K GNSS        <- top (UART TX/RX via header pins)
XIAO MCU         <- middle
Wio-SX1262       <- bottom (SPI via B2B connector)
```

Grove Shield attaches to XIAO side pins alongside the stack. Verify mechanical clearance with L76K.

### BOM — base sensors (same as B1, with one swap)

All B1 sensors carry over, except:

- **DFR0300-H EC (K=10)** replaces Grove TDS — K=10 rated for seawater (0-100 mS/cm vs freshwater-only ~2000 uS/cm)

### BOM — additions over B1

|Component|Interface|VCC|Library / Docs|Calibration & Notes|Status|
|---|---|---|---|---|---|
|[L76K GNSS](https://www.seeedstudio.com/L76K-GNSS-Module-for-Seeed-Studio-XIAO-p-5864.html)|UART Serial1 (stacked)|3.3V|[TinyGPS++](https://github.com/mikalhart/TinyGPSPlus)|No calibration. Duty-cycleable — hot start ~2 s.|**In stock**|
|[Reolink SP2-W](https://mauser.pt/095-4494/painel-solar-6w-para-camaras-a-bateria-reolink-5v-usb-c-conversor-micro-usb-211-175-15mm) 6W Solar Panel|5V USB-C out|--|--|IP65, 211x175x15 mm. Cut USB-C -> wire to DFR0559 solar input.|**TO ORDER**|
|[DFRobot DFR0559](https://www.dfrobot.com/product-1712.html) Solar Power Manager 5V|--|5V solar / 3.7V LiPo|[DFR0559 wiki](https://wiki.dfrobot.com/Solar_Power_Manager_5V_SKU__DFR0559)|MPPT. Up to 900 mA charge. USB + solar input. Regulated 5V/3.3V output.|**TO ORDER**|
|[DFRobot DFR0300-H](https://www.dfrobot.com/product-1797.html) EC (K=10)|Analog|--|[Wiki](https://wiki.dfrobot.com/Gravity_Analog_Electrical_Conductivity_Sensor_Meter_K=10_SKU_DFR0300-H)|K=10 rated for seawater. Replaces Grove TDS.|**Will purchase**|
|[DFRobot SEN0237](https://www.dfrobot.com/product-1628.html) Dissolved Oxygen|Analog|--|[Wiki](https://wiki.dfrobot.com/Gravity__Analog_Dissolved_Oxygen_Sensor_SKU_SEN0237)|Galvanic probe. Fresh + saltwater with salinity compensation. Membrane cap replacement every 1-5 months.|**Will purchase**|

> **pH in B2:** Keeping Phidgets ASP200 + 1130. Saltwater strategy: automated sample collection cycles to limit continuous immersion time — to be tested. Atlas Scientific Surveyor remains the fallback if drift is unacceptable. Note: the sampling mechanism (pump/valve/chamber) is the most mechanically complex sub-system in B2.

**Why this sensor set matters:** EC, pH, DO, temperature, and turbidity are the standard parameter set for bathing water quality (EPA, APA-PT). A buoy with these sensors produces data directly comparable to official monitoring stations.

**Cost delta B2 vs B1:** ~180 EUR for EC K=10 + DO sensor + solar panel + DFR0559.

---

## Project A — Air Quality Helmet

> **Deferred — not part of Spring 2026 coursework.** Wearable air quality monitoring during urban bike commuting. Documented here for continuity. **Comms: Meshtastic / MeshCore** mesh network. See [[Communication#Meshtastic / MeshCore (B2, A)|below]].

### BOM

|Component|Interface|VCC|Library / Docs|Calibration & Notes|
|---|---|---|---|---|
|[SPS30](https://www.sensirion.com/sps30/) Particulate Matter|I2C `0x69`|5V|[paulvha/SPS30](https://github.com/paulvha/sps30)|Factory calibrated. Run 30 min in clean air after storage. Use `sleep()`/`wakeUp()` between readings to cut fan power.|
|[PMSA003I](https://www.adafruit.com/product/4505) Particulate Matter|I2C `0x12`|3.3/5V|[Adafruit PM25AQI](https://github.com/adafruit/Adafruit_PM25AQI)|Factory calibrated. 30 s stabilisation on power-up before valid data.|
|[MH-Z16](https://www.winsen-sensor.com/sensors/co2-sensor/mh-z16.html) CO2 NDIR|UART Serial0|**5V**|[Winsen datasheet](https://www.winsen-sensor.com/d/files/infrared-gas-sensor/mh-z16-co2-sensor\(0.1ppm-10000ppm\)manual\(ver1.4\).pdf)|**3 min warm-up mandatory on power-on.** ABC auto-cal on by default — disable for urban/indoor. Sleep mode available; at 1 read/10 min, avg draw ~15 mA.|
|[SGP30](https://www.adafruit.com/product/3709) VOC/eCO2|I2C `0x58`|3.3V|[Adafruit SGP30](https://github.com/adafruit/Adafruit_SGP30)|**Do not power-cycle between reads** — continuous operation required for valid baseline. First use: run 12-24 h, save baseline to NVM. Reload on every boot. Discard stored baseline if >7 days old.|
|[L76K GNSS](https://www.seeedstudio.com/L76K-GNSS-Module-for-Seeed-Studio-XIAO-p-5864.html)|UART Serial1 (stacked)|3.3V|[TinyGPS++](https://github.com/mikalhart/TinyGPSPlus)|Hot start ~2 s. Stacked on top of XIAO via header pins.|
|5V boost converter|--|LiPo in|--|Powers MH-Z16 + SPS30|
|Level shifter (MH-Z16 TX -> 3.3V)|--|--|--|Mandatory — protects ESP32 RX pin|
|LiPo 1S 2000 mAh|--|--|--|~8-12 h runtime at 1 read/10 min|

### UART note

MH-Z16 (Serial0) and GPS (Serial1) each need a dedicated hardware UART — works cleanly on the S3. The Wio-SX1262 uses SPI (B2B), no UART contention.

### I2C bus

SPS30 `0x69` / PMSA003I `0x12` / SGP30 `0x58` — no address conflicts, single bus.

---

## Communication

### LoRaWAN (B1)

The well sensor uses LoRaWAN via The Things Network for long-range, low-power transmission to the RPi server stack. No mesh — point-to-gateway. No GPS (fixed location).

```
B1 sensor node (XIAO + Wio-SX1262)
    | LoRaWAN (OTAA, EU 868 MHz)
    v
TTN gateway
    | TTN MQTT integration
    v
RPi 4 (steam228iot.local)
    Mosquitto -> Node-RED -> InfluxDB -> Grafana
```

- Payload: Cayenne LPP (auto-decoded by TTN payload formatter)
- RadioLib 6.6.0, `Module(41, 39, 42, 40)`

Full LoRaWAN setup: [[../SensorNodes/LoRaWAN]] Server stack: [[../Server/steam228IOTRPI4@HOME]]

### Meshtastic / MeshCore (B2, A)

The buoy and helmet use [Meshtastic](https://meshtastic.org/) (or [MeshCore](https://meshcore.co.uk/) as an alternative) for device-to-device and device-to-gateway communication over LoRa mesh. No TTN infrastructure required — gateway node bridges directly to MQTT on the RPi.

```
B2/A sensor node (XIAO + Wio-SX1262)
    | LoRa mesh (862-930 MHz)
    v
Meshtastic gateway node (ESP32-S3, WiFi)
    | MQTT publish (JSON) over WiFi
    v
RPi 4 (steam228iot.local)
    Mosquitto -> Node-RED -> InfluxDB -> Grafana
```

- Firmware: pre-flashed on Seeed kits ([ESP32-S3](https://www.seeedstudio.com/Wio-SX1262-with-XIAO-ESP32S3-p-5982.html) / [nRF52840](https://www.seeedstudio.com/XIAO-nRF52840-Wio-SX1262-Kit-for-Meshtastic-p-6400.html))
- Config: via Meshtastic app (BLE) or web UI (ESP32-S3 only, via WiFi)
- Telemetry module: sensor data transmitted as Meshtastic telemetry packets
- Range: typical 1-5 km line-of-sight, extendable via mesh relay nodes

> ⚠️ **Architecture caveat — Meshtastic reads I2C sensors only.** Stock Meshtastic telemetry auto-detects supported **I2C** sensors on the bus; arbitrary **analog** sensors (DFRobot EC / DO / turbidity, Phidgets pH) are not read by the stock firmware. A `CUSTOM_SENSOR` path exists but is built on the meshtastic/i2c-sensor framework and requires compiling a custom module. Practical options for B2/A:
> 
> 1. **Meshtastic as transport only** — run the analog sensors on the XIAO with your own firmware (reuse B1 sensor code) and push readings into the mesh via the Serial module / MQTT injection. _(Recommended — maximises B1↔B2 code reuse.)_
> 2. **Custom Meshtastic firmware module** — fork and implement the sensor interface (native telemetry, more work).
> 3. **Add an I2C front-end** (e.g. ADS1115) so analog sensors present as I2C — still needs a custom module to map values into telemetry.
> 
> Consequence: B1's "RadioLib reads sensors directly" firmware does **not** port unchanged onto a stock Meshtastic node. Decide the B2 comms architecture before reusing B1 firmware.

Full Meshtastic setup: [[../SensorNodes/Meshtastic]]

### Data ingestion — shared RPi Docker stack

Both LoRaWAN and Meshtastic data feed into the same server pipeline:

|Container|Image|Role|
|---|---|---|
|Mosquitto|`eclipse-mosquitto`|Local MQTT broker|
|Node-RED|`nodered/node-red`|Parses telemetry JSON, tags by device, writes to InfluxDB|
|InfluxDB|`influxdb:2`|Time-series storage|
|Grafana|`grafana/grafana`|Dashboards — time-series panels, GPS map panel (B2)|

> Server is documented as RPi 4 (steam228iot.local). If migrating to an RPi 5, update this reference and the linked server note.

Server docs: [[../Server/steam228IOTRPI4@HOME]]

---

## Fabrication Notes

- Common XIAO + Wio-SX1262 footprint across all projects
- **Sensor integration via Grove Shield for XIAO** — no custom carrier PCB for B1 (semester scope). Non-Grove sensors (DS18B20, Phidgets 1130) wired to breakout Grove connectors.
- Parametric enclosure: Fusion 360, printed in ASA (UV + chemical resistance)
- **B1 (well):** Smaller sealed hull. O-ring sealed, M12 cable glands for sensor wires, BNC gland for pH probe. No solar panel cutout.
- **B2 (buoy):** O-ring sealed hull with solar panel on top deck. M12 cable glands, BNC gland. Internal tray: DFR0559 MPPT -> LiPo -> XIAO stack. Sensor cluster on vibration-dampened mount. Biofouling protection on optical sensors.
- **A (helmet):** Snap/rail mount, ventilation geometry critical. Deferred.

---

## Shopping Lists

### B1 — Well Sensor (semester)

|Item|Store|Link|Notes|
|---|---|---|---|
|Grove Shield for XIAO|Seeed|[Seeed](https://www.seeedstudio.com/Grove-Shield-for-Seeeduino-XIAO-p-4621.html)|Replaces custom PCB|
|LiPo 3.7V 4500 mAh (JST 2-pin)|Mauser.pt|[035-5302](https://mauser.pt/035-5302/bateria-3-7v-4500mah-li-po-34x97x12mm)|Check JST polarity|
|BNC panel-mount bulkhead connector (waterproof)|Mauser.pt|Search on site|pH probe pass-through|
|M12 waterproof cable glands|Mauser.pt|Search on site|Sensor wire pass-throughs|
|O-rings|Mauser.pt|TBD after hull design|Hull seal|
|pH buffer solutions (4.0 + 7.0)|Lab supplier|--|Two-point calibration|
|TDS calibration KCl (84 + 1413 uS/cm)|Lab supplier|--|Two-point calibration|

### B2 — Open Water Buoy (after semester)

All B1 items, plus:

|Item|Store|Link|Notes|
|---|---|---|---|
|Reolink SP2-W 6W Solar Panel (5V USB-C, IP65)|Mauser.pt|[095-4494](https://mauser.pt/095-4494/painel-solar-6w-para-camaras-a-bateria-reolink-5v-usb-c-conversor-micro-usb-211-175-15mm)|Cut USB-C -> wire to DFR0559|
|DFRobot DFR0559 Solar Power Manager 5V (MPPT)|DFRobot|[DFR0559](https://www.dfrobot.com/product-1712.html)|Not at Mauser|
|DFRobot DFR0300-H EC Sensor (K=10)|DFRobot|[DFR0300-H](https://www.dfrobot.com/product-1797.html)|Seawater conductivity|
|DFRobot SEN0237 Dissolved Oxygen Sensor|DFRobot|[SEN0237](https://www.dfrobot.com/product-1628.html)|Membrane cap replacement ~yearly|

---

## Next Steps

- [x] Configured RPi 4 with IoT stack — [[../Server/steam228IOTRPI4@HOME]]
- [ ] **B1 — Setup well sensor project** (semester: LoRaWAN→TTN, Grove Shield, no GPS, no solar, waterproof hull)
- [ ] Register B1 node on TTN, verify LoRaWAN uplink
- [ ] Purchase missing parts (B1 list first)
- [ ] Deploy B1 on well, setup InfluxDB + Grafana panel
- [ ] Learn Meshtastic / MeshCore comms for B2 and A (messaging first, then sensor data)
    - https://flaresat.com/?ref=adrelien.com#install (with GPS module)
    - https://meshtastic.org/docs/hardware/devices/seeed-studio/wio-series/wio-sx1262s/
- [ ] **B2 — Open water buoy** (after semester: GPS, solar, saltwater sensors, Meshtastic/MeshCore mesh — decide transport-only vs custom firmware for analog sensors)