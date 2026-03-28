#pragma once

#include "../inc/MarlinConfig.h"

#if ENABLED(HX711_SENSOR)

typedef struct {
  pin_t dout_pin;
  pin_t sck_pin;
  int32_t offset_a;
  int32_t offset_b;
  float scale_a;
  float scale_b;
} hx711_config_t;

enum HX711Channel : uint8_t {
  HX711_CHANNEL_A_GAIN_128 = 1,  // 1 extra pulse: Channel A, Gain 128
  HX711_CHANNEL_B_GAIN_32  = 2,  // 2 extra pulses: Channel B, Gain 32
  HX711_CHANNEL_A_GAIN_64  = 3   // 3 extra pulses: Channel A, Gain 64
};

class HX711Sensor {
public:
  HX711Sensor(const hx711_config_t &cfg);

  void init();
  bool read_raw(int32_t &raw, const HX711Channel channel = HX711_CHANNEL_A_GAIN_128);
  bool read_units(float &value_a, float &value_b);
  bool read_both_channels(int32_t &channel_a, int32_t &channel_b);

private:
  hx711_config_t config;

  bool ready() const;
  void clock_pulse() const;
};

extern HX711Sensor hx711_sensors[];
extern const uint8_t hx711_sensor_count;

void hx711_init();

#endif // HX711_SENSOR
