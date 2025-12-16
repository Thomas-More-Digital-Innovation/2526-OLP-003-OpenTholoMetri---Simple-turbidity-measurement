# OpenTholoMetri - Turbidity Measurement System Documentation

## Table of Contents
1. [System Overview](#system-overview)
2. [Hardware Architecture](#hardware-architecture)
3. [Software Architecture](#software-architecture)
4. [Module Documentation](#module-documentation)
5. [Power Management](#power-management)
6. [Data Flow](#data-flow)
7. [Configuration](#configuration)
8. [Setup and Deployment](#setup-and-deployment)
9. [Troubleshooting](#troubleshooting)

---

## System Overview

The OpenTholoMetri is a data logging system designed for turbidity measurement using optical sensors. The system operates in a low-power mode, periodically waking up to take sensor readings, logging the data to an SD card with timestamps, and then returning to sleep.

### Key Features
- **Automated Data Logging**: Periodic sensor readings at configurable intervals
- **Low Power Operation**: Deep sleep mode between readings (~6µA in standby)
- **Real-Time Clock**: Accurate timestamping using PCF8523 RTC
- **SD Card Storage**: CSV format data logging with timestamps
- **Optical Sensing**: VCNL4010 proximity and ambient light sensor
- **Interrupt-Driven**: RTC timer-based wake-up mechanism

### Hardware Platform
- **Microcontroller**: SAMD21-based board (Adafruit Feather M0)
- **RTC**: PCF8523 Real-Time Clock (integrated on Adalogger FeatherWing)
- **Sensor**: VCNL4010 Proximity and Ambient Light Sensor
- **Storage**: SD Card via SPI interface (integrated on Adalogger FeatherWing)

---

## Hardware Architecture

### Pin Configuration

| Pin | Function | Description |
|-----|----------|-------------|
| 10 | SD_CS_PIN | SD card chip select (SPI) |
| 13 | LED_PIN | Status indicator LED |
| 5 | RTC_INTERRUPT_PIN | RTC countdown timer interrupt (SQW/INT) |
| I2C | Sensor/RTC | VCNL4010 sensor (0x13) and PCF8523 RTC (0x68) communication |
| SPI | SD Card | SD card data interface |

### Power Consumption

| Mode | Current Draw | Description |
|------|-------------|-------------|
| Standby (USB disconnected) | ~6µA | Deep sleep mode, waiting for RTC interrupt |
| Idle (USB connected) | ~8mA | Light sleep mode, maintains USB connection |
| Active (sensing/logging) | ~12mA | During sensor reading and SD write operations |

### External Components

#### PCF8523 RTC
- **Interface**: I2C (Address: 0x68)
- **Function**: Provides accurate timekeeping and generates periodic wake-up interrupts

#### VCNL4010 Sensor
- **Interface**: I2C (Address: 0x13)
- **Function**: Turbidity measurement in water samples
- **Range**: 
  - Proximity: 0-65535 (higher values = closer/more turbid)
  - Ambient Light: 0-65535 (lux-equivalent)

#### SD Card
- **Interface**: SPI
- **Format**: FAT16/FAT32
- **Data Format**: CSV (Comma-Separated Values)
- **File**: sensor.csv


## Data Logging Process
1. Read sensor values (proximity + ambient light)
2. Get current timestamp from RTC
3. Flash LED (visual indicator in debug mode)
4. Write CSV entry to SD card
5. Turn off LED
6. Print status to Serial
7. Return to sleep

---

## Power Management

### Power Modes

#### Active Mode
- **Current**: 20-50mA
- **Activities**: Sensor reading, SD write, Serial output

#### Sleep Mode (USB Connected)
- **Current**: ~15mA
- **Reason**: Maintains USB connection for debugging
- **Mode**: M0 Idle mode
- **Wake**: Any interrupt

#### Sleep Mode (USB Disconnected)
- **Current**: ~0.3mA
- **Mode**: SAMD21 Standby mode
- **Wake**: RTC interrupt on pin 5 only
- **Peripherals**: Most disabled, RTC active

## Configuration

### Changing Logging Interval

Edit [Config.h](main/Config.h):
```cpp
const int SLEEP_INTERVAL = 60;  // Change to 60 seconds (1 minute)
```

**Considerations**:
- Shorter intervals: More data, higher power consumption, larger files
- Longer intervals: Less data, better battery life, smaller files

### Software Installation

#### Prerequisites
```
Arduino IDE 1.8.x or 2.x
Board: Adafruit SAMD Boards (Adafruit Feather M0)
```

#### Required Libraries
Install via Arduino Library Manager:
1. **RTClib** by Adafruit
2. **Adafruit VCNL4010**
3. **SD** (built-in)
4. **SPI** (built-in)
5. **Wire** (built-in)

#### Upload Process
1. Open [main.ino](main/main.ino) in Arduino IDE
2. Select board: Tools → Board → Adafruit Feather M0
3. Select port: Tools → Port → [Your COM port]
4. Click Upload
5. Open Serial Monitor (9600 baud) to verify initialization

### First-Time Setup

1. **Set RTC Time**
   - First upload sets RTC to compilation time

2. **Verify SD Card**
   - Check Serial output for "SD card initialized successfully!"
   - Confirm "sensor.csv" created on SD card
   - Verify CSV headers present

3. **Test Sensor**
   - Check Serial output for "VCNL4010 sensor initialized successfully!"
   - Verify proximity and light readings appear reasonable

4. **Test Logging Cycle**
   - Wait for first RTC interrupt (up to SLEEP_INTERVAL seconds)
   - Verify LED flash and "Data logged" message
   - Check SD card for new entries

### Deployment Configuration

For field deployment:
1. Set `DEBUG_MODE = false` in [Config.h](main/Config.h)
2. Set desired `SLEEP_INTERVAL` in [Config.h](main/Config.h)
3. Ensure battery backup in RTC (CR1220 coin cell)
4. Format fresh SD card (FAT32, >1GB recommended)
5. Upload final firmware
6. Connect battery and deploy

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-12-16 | David Maat | Initial documentation |

---

## References

### Related Resources
- [Adafruit Feather M0 Guide](https://learn.adafruit.com/adafruit-feather-m0-basic-proto/overview)
- [Low Power Adafruit M0](https://github.com/ProjectsByJRP/M0_Sleep_External_Int/blob/master/sleep_with_ext_int_pin6.ino)
- [OpenOBS-328](https://github.com/tedlanghorst/OpenOBS-328/tree/main)

---

**End of Documentation**
