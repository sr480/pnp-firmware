# HX711 Sensor Support - M901 Command

## Overview
This implementation adds support for HX711 load cell amplifiers to Marlin firmware via the M901 G-code command. The HX711 uses a two-wire serial protocol (DOUT/SCK), not I2C.

## Files Modified/Created

### Core Implementation
- **Marlin/src/feature/hx711.h** - HX711 sensor driver header
- **Marlin/src/feature/hx711.cpp** - HX711 sensor driver implementation
- **Marlin/src/gcode/feature/sensors/M901.cpp** - M901 G-code command handler

### Configuration
- **Marlin/Configuration_adv.h** - Added HX711_SENSOR configuration section
- **Marlin/src/inc/Conditionals_adv.h** - Added conditional compilation logic

### Integration
- **Marlin/src/gcode/gcode.h** - Added M901 command declaration
- **Marlin/src/gcode/gcode.cpp** - Added M901 to command dispatch table
- **Marlin/src/MarlinCore.cpp** - Added HX711 initialization call
- **Marlin/src/pins/ramps/pins_RAMPS.h** - Added default pin definitions
- **ini/features.ini** - Added build system integration

## How to Enable

1. Open `Marlin/Configuration_adv.h`
2. Find and uncomment the following line (around line 4050):
   ```cpp
   #define HX711_SENSOR
   ```

3. Configure your sensor pins and calibration values:
   ```cpp
   #define HX711_1_DOUT_PIN 53    // Data output pin
   #define HX711_1_SCK_PIN  51    // Clock pin
   #define HX711_1_OFFSET_A   0     // Tare offset (raw value at zero load)
   #define HX711_1_SCALE_A    1.0   // Scale factor (raw units to display units)
   #define HX711_1_OFFSET_B   0     // Tare offset (raw value at zero load)
   #define HX711_1_SCALE_B    1.0   // Scale factor (raw units to display units)
   ```

4. For a second sensor (optional):
   ```cpp
   #define HX711_2_DOUT_PIN 49
   #define HX711_2_SCK_PIN  47
   #define HX711_2_OFFSET   0
   #define HX711_2_SCALE    1.0
   ```

5. Optionally customize:
   ```cpp
   #define HX711_UNITS_LABEL "g"           // Units label for display
   #define HX711_READY_TIMEOUT_MS 100      // Timeout for sensor ready signal
   ```

## Pin Configuration for RAMPS

**Note:** HX711 does NOT use I2C. It uses a two-wire serial protocol:
- **DOUT (Data Out)** - Connect to any digital input pin
- **SCK (Serial Clock)** - Connect to any digital output pin

Default pins for RAMPS (can be changed in Configuration_adv.h):
- Sensor 1: DOUT=53, SCK=51
- Sensor 2: DOUT=49, SCK=47 (if enabled)

The original request mentioned pins 20-21 (I2C), but HX711 sensors don't use I2C. You can configure any available digital pins.

## M901 Command Usage

### Read all sensors (Channel A, default)
```
M901
```
Output example:
```
HX711[0]: 123.45 kPa 132.21 kPa
```

### Read specific sensor
```
M901 S0    ; Read sensor 0
```

### Read raw values (no calibration applied)
```
M901 R     ; Read sensor's raw values
```
Output example:
```
HX711[0] 8388607 322344
```

## Calibration Process

1. **Find the tare offset:**
   ```
   M901 S0 R    ; Read raw value with no load
   ```
   Set this value as `HX711_1_OFFSET_A` and `HX711_1_OFFSET_B` in Configuration_adv.h

2. **Find the scale factor:**
   - Place a known weight on the sensor
   - Read the raw value: `M901 S0 R`
   - Calculate: `scale = known_weight / (raw_value - offset)`
   - Set this as `HX711_1_SCALE` in Configuration_adv.h

3. **Verify:**
   ```
   M901 S0    ; Should now display the correct weight
   ```

## Technical Details

- **Protocol:** Two-wire serial (not I2C)
- **Resolution:** 24-bit ADC
- **Gain:** 128 (Channel A) - hardcoded in this implementation
- **Timeout:** Configurable, default 100ms
- **Multiple Sensors:** Supports up to 2 sensors (easily expandable)

## Troubleshooting

### Reading 0x7FFFFF (8388607) - All bits HIGH
This indicates the DOUT pin is reading HIGH for all 24 bits. Possible causes:
1. **Wrong DOUT pin** - Verify the DOUT pin number in Configuration_adv.h matches your wiring
2. **Sensor not powered** - Check VCC and GND connections to the HX711
3. **Sensor not ready** - The HX711 needs time to power up (100ms delay added in init)
4. **Inverted logic** - Some HX711 modules may have inverted signals
5. **Timing issues** - Try increasing delays in the code if needed

To diagnose:
- Check that SCK is connected (you mentioned disconnecting SCK gives 0, which is good)
- Verify DOUT pin with a multimeter - it should go LOW when sensor is ready
- Try different pins if available
- Check sensor power supply (2.7V-5.5V)

### Reading 0x000000 or 0x800000
- **0x000000**: DOUT pin always LOW - check wiring or try different pin
- **0x800000**: Minimum negative value - sensor may be saturated or damaged

### Timeout errors
- Check wiring connections
- Increase `HX711_READY_TIMEOUT_MS` in Configuration_adv.h
- Verify sensor is powered

### Incorrect readings
- Recalibrate offset and scale values using the calibration process above
- Ensure load cell is properly mounted and not binding

### Build errors
- Ensure `HX711_SENSOR` is defined in Configuration_adv.h
- Check that all pin definitions are valid for your board

### Pin conflicts
- Choose different pins if defaults conflict with your setup
- Avoid using pins already assigned to other functions
