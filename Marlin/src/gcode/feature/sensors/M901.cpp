#include "../../../inc/MarlinConfig.h"

#if ENABLED(HX711_SENSOR)

#include "../../gcode.h"
#include "../../../feature/hx711.h"

/**
 * M901: Read HX711 load cell sensor(s)
 *
 * Usage:
 *   M901       - Read all configured HX711 sensors (Channel A Channel B)
 *   M901 S<n>  - Read sensor <n> (0-based index)
 *   M901 R     - Read raw values (no offset/scale applied)
 */
void GcodeSuite::M901()
{
  const bool use_raw = parser.seen('R');
  const uint8_t channel_select = parser.byteval('C', 0);
  uint8_t sensor_idx = 0;

  if (parser.seenval('S'))
  {
    sensor_idx = parser.value_byte();

    if (sensor_idx >= hx711_sensor_count)
    {
      SERIAL_ECHOLNPGM("Invalid sensor index: ", sensor_idx);
      return;
    }
  }

  if(use_raw) {
    int32_t value_a, value_b;
    if (hx711_sensors[sensor_idx].read_both_channels(value_a, value_b))
    {
      SERIAL_ECHOLNPGM("HX711[", sensor_idx, "]: ", value_a, " ", value_b);
    }
    else
    {
      SERIAL_ECHOLNPGM("HX711[", sensor_idx, "] Read timeout");
    }
  } else {
    float value_a, value_b;
    if (hx711_sensors[sensor_idx].read_units(value_a, value_b))
    {
      SERIAL_ECHOLNPGM("HX711[", sensor_idx, "]: ", value_a, " ", HX711_UNITS_LABEL, " ", value_b, " ", HX711_UNITS_LABEL);
    }
    else
    {
      SERIAL_ECHOLNPGM("HX711[", sensor_idx, "] Read timeout");
    }
  }
}

#endif // HX711_SENSOR
