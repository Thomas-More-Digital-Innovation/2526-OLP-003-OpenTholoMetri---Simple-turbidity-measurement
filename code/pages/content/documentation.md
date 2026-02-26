# Code Documentation

## Overview

The OpenTholoMetri firmware runs on an Adafruit Feather M0 microcontroller and performs:
- Periodic sensor readings at configurable intervals (default: 1 minute)
- Timestamped data logging to SD card in CSV format
- Deep sleep between measurements for power efficiency (~6µA in standby)
- Interrupt-driven wake-up via RTC timer

### Key Features
- **Automated Data Logging**: Periodic sensor readings with configurable intervals
- **Low Power Operation**: Deep sleep mode between readings
- **Real-Time Clock**: Accurate timestamping using PCF8523 RTC
- **Optical Sensing**: VCNL4010 proximity and ambient light sensor
- **Temperature Monitoring**: DS18B20 waterproof temperature probe
- **Power Monitoring**: Battery voltage tracking
- **Interrupt-Driven**: RTC timer-based wake-up mechanism

## Hardware Architecture

### Microcontroller
- **Platform**: SAMD21-based (Adafruit Feather M0)
- **RTC Module**: PCF8523 (on Adalogger FeatherWing, I2C address 0x68)
- **Storage**: SD Card via SPI (on Adalogger FeatherWing)

### Pin Configuration

| Pin | Function | Description |
|-----|----------|-------------|
| 10 | SD_CS_PIN | SD card chip select (SPI) |
| SPI | SCK/MOSI/MISO | SD card data interface (hardware SPI pins) |
| I2C | SDA/SCL | VCNL4010 (0x13) and PCF8523 (0x68) |
| 5 | RTC_INTERRUPT_PIN | RTC countdown timer interrupt (SQW/INT from RTC) |
| 9 | BATTERY_PIN | Battery voltage ADC input (via /2 divider) |
| 12 | DS18B20_POWER_PIN | DS18B20 VCC power control |
| A0 | DS18B20_DATA_PIN | DS18B20 1‑Wire data |
| 13 | GROVE_POWER_PIN | Grove turbidity sensor VCC power control |
| A1 | GROVE_ANALOG_PIN | Grove turbidity sensor analog ADC input |

### Power Consumption

| Mode | Current | Description |
|------|---------|-------------|
| Standby (USB disconnected) | ~6µA | Deep sleep, waiting for RTC interrupt |
| Idle (USB connected) | ~15mA | Light sleep, maintains USB for debugging |
| Active (sensing/logging) | ~12-20mA | Sensor reading and SD write operations |

## Firmware Structure

The code is organized into modular components:

- **`main.ino`** - Main loop handling sleep, RTC interrupts, and sensor reads
- **`RTCManager`** - Manages real-time clock and timer interrupts
- **`SensorManager`** - Reads all sensors (proximity, light, temperature, battery)
- **`SDLogger`** - Writes timestamped data to SD card in CSV format

## Sensor Data

The system reads the following sensors:

- **VCNL4010**: Proximity and ambient light (I2C address 0x13)
  - Proximity: 0-65535 (higher = more turbid)
  - Ambient Light: 0-65535 (lux-equivalent)
- **Grove Turbidity Sensor**: Raw ADC value and calculated voltage
- **DS18B20**: Water temperature in Celsius
- **Battery Monitor**: Supply voltage monitoring

## CSV Output Format

Data is logged with the following columns:

```
Timestamp,Proximity,Ambient Light,Battery Voltage,Temperature (C),Turbidity Raw,Turbidity Voltage
2024-01-15 10:30:00,245,1023,4.15,22.5,512,2.50
```

- **Timestamp**: YYYY-MM-DD HH:MM:SS format
- **Proximity/Ambient Light**: 0-65535
- **Battery Voltage**: Volts
- **Temperature**: Celsius
- **Turbidity Raw**: Raw ADC value
- **Turbidity Voltage**: Calculated sensor voltage

## Configuration

### Changing Logging Interval

Edit `Config.h`:
```cpp
const int SLEEP_INTERVAL = 300;  // Change to desired seconds (300 = 5 minutes)
```

**Considerations**:
- Shorter intervals: More data collection, higher power consumption, larger files
- Longer intervals: Less frequent data, better battery life, smaller files

### Software Installation

#### Required Libraries
Install via Arduino Library Manager:
1. **RTClib** by Adafruit
2. **Adafruit VCNL4010**
3. **SD** (built-in)
4. **SPI** (built-in)
5. **Wire** (built-in)
6. **OneWire**
7. **DallasTemperature**

#### Upload Process
1. Open `main.ino` in Arduino IDE
2. Select board: Adafruit Feather M0
3. Select port: Your COM port
4. Click Upload
5. Open Serial Monitor (115200 baud) to verify initialization

### First-Time Setup

1. **Verify RTC**: Check Serial output shows RTC initialization success
2. **Verify SD Card**: Confirm "SD card initialized successfully!" and `sensor.csv` is created
3. **Test Sensor**: Check for reasonable proximity and light readings
4. **Test Logging**: Wait for first RTC interrupt and verify LED flash and data written to SD card

### Deployment Configuration

For field deployment:
1. Set desired `SLEEP_INTERVAL` in `Config.h`
2. Ensure CR1220 coin cell battery in RTC socket
3. Format fresh SD card (FAT32)
4. Upload final firmware
5. Connect battery and deploy
