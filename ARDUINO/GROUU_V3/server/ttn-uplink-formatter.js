// WellBouy — TTN v3 custom UPLINK payload formatter (CayenneLPP subset).
// Paste into: TTN Console → your device (or application) →
//   Payload formatters → Uplink → "Custom Javascript formatter".
//
// Decodes the LPP types this node uses AND renames them to semantic fields
// (temperature, tds, turbidity, ph) so decoded_payload is human-readable all
// the way through Node-RED / InfluxDB / Grafana.

// channel-scoped names for this node (LPP type carries the value; we label it).
// TDS and turbidity ride on Luminosity (0x65, 0-65535) because Analog Input
// (0x02) overflows above 327.67 — both can exceed that. pH fits Analog Input.
var FIELD_NAMES = {
  "temperature_1": "temperature", // ch1 · DS18B20 water temp (°C)
  "luminosity_4":  "tds",         // ch4 · TDS (ppm)      — luminosity type
  "luminosity_3":  "turbidity",   // ch3 · turbidity (NTU) — luminosity type
  "analog_in_2":   "ph"           // ch2 · pH (0-14)      — analog-input type
};

function decodeUplink(input) {
  var bytes = input.bytes;
  var raw = {};
  var warnings = [];
  var i = 0;

  while (i + 1 < bytes.length) {
    var ch = bytes[i++];
    var type = bytes[i++];

    if (type === 0x00) {                     // Digital Input (1 byte)
      raw["digital_in_" + ch] = bytes[i];
      i += 1;
    } else if (type === 0x02) {              // Analog Input (2 bytes signed, ×0.01)
      var a = (bytes[i] << 8) | bytes[i + 1];
      if (a & 0x8000) a -= 0x10000;
      raw["analog_in_" + ch] = a / 100;
      i += 2;
    } else if (type === 0x65) {              // Luminosity (2 bytes unsigned)
      raw["luminosity_" + ch] = (bytes[i] << 8) | bytes[i + 1];
      i += 2;
    } else if (type === 0x67) {              // Temperature (2 bytes signed, ×0.1)
      var t = (bytes[i] << 8) | bytes[i + 1];
      if (t & 0x8000) t -= 0x10000;
      raw["temperature_" + ch] = t / 10;
      i += 2;
    } else if (type === 0x88) {              // GPS (9 bytes) — skip
      i += 9;
    } else {
      warnings.push("unknown LPP type 0x" + type.toString(16) + " at byte " + (i - 1));
      break;                                 // stop to avoid mis-parsing
    }
  }

  // rename to semantic fields; anything unmapped passes through unchanged
  var data = {};
  for (var key in raw) { data[FIELD_NAMES[key] || key] = raw[key]; }

  return { data: data, warnings: warnings };
}