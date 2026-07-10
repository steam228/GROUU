// ============================================================================
//  config.h - Pins, timing, calibration constants and debug helpers.
//
//  Everything that you might tune in the field lives here. Secrets (LoRaWAN
//  keys) are kept separately in include/secrets.h.
// ============================================================================
#pragma once

#include <Arduino.h>

// ----------------------------------------------------------------------------
//  Debug
// ----------------------------------------------------------------------------
//  Toggle with -D DEBUG=1 / 0 in platformio.ini. When disabled, DBG() compiles
//  to nothing and the Serial port is never opened (saves power on the node).
#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
  #define DBG(...)    Serial.printf(__VA_ARGS__)
  #define DBG_BEGIN() do { Serial.begin(115200); \
                           uint32_t _t = millis(); \
                           while (!Serial && (millis() - _t) < 2000) {} } while (0)
  #define DBG_FLUSH() Serial.flush()
#else
  #define DBG(...)    do {} while (0)
  #define DBG_BEGIN() do {} while (0)
  #define DBG_FLUSH() do {} while (0)
#endif

// ----------------------------------------------------------------------------
//  Transmit / sleep timing
// ----------------------------------------------------------------------------
//  One uplink every TX_INTERVAL_S seconds; the node deep-sleeps in between.
//  This is only the COLD-BOOT default: main.cpp keeps the live interval in RTC
//  memory and a downlink command (0x01 <minutes>) can change it at runtime.
//  NOTE: keep within the TTN fair-use policy for your spreading factor.
//  Overridable at build time (e.g. -DTX_INTERVAL_S=120 for a quick bench test).
#ifndef TX_INTERVAL_S
#define TX_INTERVAL_S        1800         // default: 30 minutes
#endif
#define TX_INTERVAL_MIN_S    60           // floor a downlink may set (duty-cycle safety)
#define LPP_FPORT            1            // LoRaWAN port used for the uplink

//  Downlink command opcodes (byte 0 of a queued TTN downlink).
#define DL_CMD_SET_INTERVAL  0x01         // + 1 byte: new interval in MINUTES (1..255)
#define DL_CMD_REBOOT        0x02         // no args: soft-reboot the node

//  Minimum gap between consecutive analog reads (datasheet: settle the ADC mux
//  and let the 5 V sensors stabilise). Spec requires > 50 ms.
#define SENSOR_GAP_MS        60

//  After enabling the 5 V rail, wait this long before the first read so the
//  turbidity / TDS analog front-ends settle.
#define SENSOR_WARMUP_MS     1000

// ----------------------------------------------------------------------------
//  Sensor power rail
// ----------------------------------------------------------------------------
//  The analog sensors run from a switched 5 V rail (driven by a MOSFET / load
//  switch) so they draw nothing during deep sleep. Set the GPIO that controls
//  that switch here, or set to -1 if the rail is always on.
//  BOARD: xiao.D0 ~ sensor_pwr_en = GPIO1 (main.ato). Must NOT be GPIO5/6 -
//  those are the ADS1115's I2C SDA/SCL and driving them as an output kills the
//  bus. On the breadboard (no load switch) GPIO1 is unconnected -> harmless.
#define SENSOR_PWR_EN_PIN        1        // D0 / GPIO1 - 5 V load-switch enable
#define SENSOR_PWR_ACTIVE_HIGH   1        // 1 = HIGH enables the rail

// ----------------------------------------------------------------------------
//  Analog sensor pins (XIAO ESP32-S3, all on ADC1)
//  Sensors are powered at 5 V; their analog outputs MUST be scaled to <= 3.3 V
//  before reaching these pins (see the turbidity divider below).
// ----------------------------------------------------------------------------
//  The three analog water-quality sensors are read through an external I2C
//  ADS1115 (16-bit), NOT the XIAO's own ADC - this is what the board design and
//  all bring-up use. The PIN_* below are the legacy direct-ADC pins, kept only
//  so the *direct* readTDS()/readTurbidity()/readPH() helpers still compile.
#define ADS_CH_TDS           0            // ADS1115 A0 - Grove TDS
#define ADS_CH_TURBIDITY     1            // ADS1115 A1 - SEN0189 (via divider)
#define ADS_CH_PH            2            // ADS1115 A2 - Phidgets 1130 pH (via divider)

#define PIN_TDS              1            // (legacy direct ADC) D0 / GPIO1
#define PIN_TURBIDITY        2            // (legacy direct ADC) D1 / GPIO2
#define PIN_PH               3            // (legacy direct ADC) D2 / GPIO3
#define PIN_ONEWIRE          4            // D3  / GPIO4  - DS18B20 (4.7k pullup)

//  I2C bus for the ADS1115 (XIAO Grove / default Wire pins).
#define ADS_I2C_HZ           100000
#define ADS_SAMPLES          8            // reads averaged per ADS channel

//  ADC sampling (per read, internal averaging) - legacy direct-ADC path.
#define ADC_SAMPLES          16
#define ADC_SAMPLE_GAP_MS    2

// ----------------------------------------------------------------------------
//  Wio-SX1262 radio wiring  ->  RadioLib Module(NSS, DIO1, RST, BUSY)
// ----------------------------------------------------------------------------
#define LORA_NSS             41
#define LORA_DIO1            39
#define LORA_RST             42
#define LORA_BUSY            40

// ----------------------------------------------------------------------------
//  Fixed location (no GPS on this node). Decimal degrees, metres.
//  Edit these for each deployed node before flashing.
// ----------------------------------------------------------------------------
#define FIXED_LATITUDE       52.379189f   // e.g. site latitude
#define FIXED_LONGITUDE      4.899431f    // e.g. site longitude
#define FIXED_ALTITUDE       0.0f         // metres above sea level

// ============================================================================
//  CALIBRATION
//  ---------------------------------------------------------------------------
//  Recalibrate these against reference standards. The procedures are in the
//  README. All voltages are at the ADC pin unless stated otherwise.
// ============================================================================

// --- TDS (DFRobot Gravity algorithm, temperature compensated to 25 C) -------
//  TDS_KVALUE is the probe constant from a TWO-POINT calibration: measure two
//  standard solutions, solve k for each, and average. Default 1.0 = factory.
#define TDS_KVALUE           1.000f

// --- pH (two-point: pH 4.0 and pH 7.0 buffers) ------------------------------
//  Voltages are measured AT THE ADS PIN (post 10k/18k divider); the two-point
//  fit absorbs the divider ratio, so no ratio constant is needed. This adapter
//  runs "acidic = lower voltage" (pH 4 < pH 7). Measured 2026-07-08 on the
//  bench rig. Re-run the pH4/pH7 dip and update if the probe/electrode changes.
#define PH_CAL_PH4           4.00f
#define PH_CAL_PH7           7.00f
#define PH_CAL_V4            1.078f        // volts at ADS in pH 4.0 buffer
#define PH_CAL_V7            1.591f        // volts at ADS in pH 7.0 buffer

// --- Turbidity (SEN0189 through a 10k / 6.8k divider) -----------------------
//  Divider ratio = R2 / (R1 + R2) = 6.8k / (10k + 6.8k) = 0.4048.
//  We divide the measured ADC voltage by this to recover the raw 0-4.5 V
//  sensor output, then apply DFRobot's quadratic voltage->NTU curve.
//
//  V_CLEAR is the MEASURED clear-water sensor voltage on this rig (5 V supply):
//  clear water read A1 = 1606 mV -> 1606/0.4048 = 3.96 V. DFRobot's raw curve
//  only crosses 0 NTU at ~4.20 V, so we rescale V_CLEAR onto that 0 point
//  (see turbidity_sensor.cpp) to get a continuous 0-at-clear response.
//  Re-measure in clear water and update V_CLEAR if the sensor/supply changes.
#define TURBIDITY_DIVIDER_RATIO   (6.8f / (10.0f + 6.8f))
#define TURBIDITY_V_CLEAR         3.96f    // MEASURED sensor V in clear water => 0 NTU
#define TURBIDITY_V_DFR0          4.20f    // sensor V at which DFRobot's curve = 0 NTU
