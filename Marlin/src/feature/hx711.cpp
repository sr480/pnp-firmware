#include "../inc/MarlinConfig.h"

#if ENABLED(HX711_SENSOR)

#include "hx711.h"
#include "../MarlinCore.h"

#ifndef HX711_READY_TIMEOUT_MS
  #define HX711_READY_TIMEOUT_MS 100
#endif

static const hx711_config_t hx711_config[] = {
  { HX711_1_DOUT_PIN, HX711_1_SCK_PIN, HX711_1_OFFSET_A, HX711_1_OFFSET_B, HX711_1_SCALE_A, HX711_1_SCALE_B },
  #if defined(HX711_2_DOUT_PIN) && defined(HX711_2_SCK_PIN)
    { HX711_2_DOUT_PIN, HX711_2_SCK_PIN, HX711_2_OFFSET, HX711_2_SCALE },
  #endif
};

HX711Sensor hx711_sensors[] = {
  HX711Sensor(hx711_config[0])
  #ifdef HX711_2_DOUT_PIN
    , HX711Sensor(hx711_config[1])
  #endif
};

const uint8_t hx711_sensor_count = COUNT(hx711_sensors);

HX711Sensor::HX711Sensor(const hx711_config_t &cfg) : config(cfg) {}

void HX711Sensor::init() {
  pinMode(config.dout_pin, INPUT);
  pinMode(config.sck_pin, OUTPUT);
  extDigitalWrite(config.sck_pin, LOW);
  safe_delay(100); // Give sensor time to stabilize
}

bool HX711Sensor::ready() const {
  return extDigitalRead(config.dout_pin) == LOW;
}

void HX711Sensor::clock_pulse() const {
  extDigitalWrite(config.sck_pin, HIGH);
  delayMicroseconds(1);
  extDigitalWrite(config.sck_pin, LOW);
  delayMicroseconds(1);
}

bool HX711Sensor::read_raw(int32_t &raw, const HX711Channel channel) {
  const millis_t start_ms = millis();
  while (!ready()) {
    if (ELAPSED(millis(), start_ms + HX711_READY_TIMEOUT_MS)) return false;
    safe_delay(1);
  }

  uint32_t data = 0;
  
  // Read 24 bits of data
  for (uint8_t i = 0; i < 24; ++i) {
    extDigitalWrite(config.sck_pin, HIGH);
    delayMicroseconds(1);
    data = (data << 1) | (extDigitalRead(config.dout_pin) ? 1 : 0);
    extDigitalWrite(config.sck_pin, LOW);
    delayMicroseconds(1);
  }

  // Send additional pulses to set channel and gain for next reading
  for (uint8_t i = 0; i < channel; ++i) {
    clock_pulse();
  }

  // Convert 24-bit two's complement to 32-bit signed
  if (data & 0x800000) {
    raw = static_cast<int32_t>(data | 0xFF000000UL);
  } else {
    raw = static_cast<int32_t>(data);
  }
  
  return true;
}

bool HX711Sensor::read_units(float &value_a, float &value_b) {
  int32_t channel_a, channel_b;
  
  if(!read_both_channels(channel_a, channel_b)) return false;

  value_a = (channel_a) * config.scale_a;
  value_b = (channel_b) * config.scale_b;
  return true;
}

bool HX711Sensor::read_both_channels(int32_t &channel_a, int32_t &channel_b) {
  // Read channel A first
  if (!read_raw(channel_a, HX711_CHANNEL_B_GAIN_32)) return false;
  
  // Wait for next conversion
  safe_delay(100);
  
  // Read channel B
  if (!read_raw(channel_b, HX711_CHANNEL_A_GAIN_64)) return false;
  
  return true;
}

void hx711_init() {
  for (uint8_t i = 0; i < hx711_sensor_count; ++i)
    hx711_sensors[i].init();
}

#endif // HX711_SENSOR
