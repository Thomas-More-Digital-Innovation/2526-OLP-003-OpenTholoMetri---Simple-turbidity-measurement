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
   - For accurate time:
     ```cpp
     // In RTCManager.cpp, temporarily set to current time:
     rtc.adjust(DateTime(2025, 12, 16, 10, 30, 0));  // Y,M,D,H,M,S
     ```
   - Upload once, then remove or comment out
   - With battery backup, time persists across power cycles

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

## Troubleshooting

### RTC Initialization Failed

**Symptoms**: "RTC initialization failed!" in Serial Monitor

**Causes & Solutions**:
- **I2C connection issue**: Check SDA/SCL wiring if using external RTC
- **Adalogger RTC**: Should work out-of-box, may indicate hardware fault
- **Power issue**: Ensure 3.3V supply is stable

**Diagnostic**:
```cpp
// Add to RTCManager::begin() to test I2C:
Wire.beginTransmission(0x68);  // PCF8523 address
int error = Wire.endTransmission();
Serial.print("I2C error code: ");
Serial.println(error);  // 0 = success
```

### SD Card Initialization Failed

**Symptoms**: "SD card initialization failed!" in Serial Monitor

**Causes & Solutions**:
- **No SD card**: Insert SD card
- **Wrong format**: Format as FAT16 or FAT32
- **Corrupted filesystem**: Reformat SD card
- **SPI pin conflict**: Verify SD_CS_PIN = 10 on Adalogger
- **Card too large**: Some cards >32GB may not work

**Test**:
```cpp
// Verify SPI manually:
pinMode(SD_CS_PIN, OUTPUT);
digitalWrite(SD_CS_PIN, LOW);
SPI.transfer(0xFF);  // Should return 0xFF or 0x01
digitalWrite(SD_CS_PIN, HIGH);
```

### Sensor Initialization Failed

**Symptoms**: "VCNL4010 sensor initialization failed!" in Serial Monitor

**Causes & Solutions**:
- **Not connected**: Check I2C wiring (SDA, SCL, VIN, GND)
- **Wrong address**: VCNL4010 uses 0x13, verify with I2C scanner
- **Power issue**: Ensure 3.3V connected
- **Faulty sensor**: Test with known-good sensor

**I2C Scanner Code**:
```cpp
// Temporarily add to setup():
Wire.begin();
for(byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if(Wire.endTransmission() == 0) {
        Serial.print("Device found at 0x");
        Serial.println(addr, HEX);
    }
}
```

### System Not Waking from Sleep

**Symptoms**: First log works, then system doesn't wake up

**Causes & Solutions**:
- **Interrupt not configured**: Verify EIC->WAKEUP setup in [main.ino](main/main.ino#L41-L57)
- **Wrong pin**: Ensure RTC_INTERRUPT_PIN = 5 and physically connected
- **Timer not running**: Check RTC timer setup
- **Pullup missing**: Verify INPUT_PULLUP on interrupt pin

**Debug**:
```cpp
// Disable sleep temporarily to test timer:
// In loop(), comment out all sleep code
// Watch for "RTC INTERRUPT FIRED!" every SLEEP_INTERVAL seconds
```

### Incorrect Timestamps

**Symptoms**: Wrong date/time in log file

**Causes & Solutions**:
- **No battery backup**: Time resets on power loss
- **Compilation time used**: Set time explicitly after upload
- **Clock drift**: PCF8523 drift ~2-3 ppm, adjust periodically

**Manual Time Set**:
```cpp
// In RTCManager::begin(), add:
rtc.adjust(DateTime(2025, 12, 16, 14, 30, 0));  // Set current time
```

### No Data Being Logged

**Symptoms**: System runs but CSV file empty or not growing

**Causes & Solutions**:
- **File not closing**: Ensure `dataFile.close()` called
- **SD card removed**: Check card is seated properly
- **SD card full**: Check available space
- **Write protection**: Remove write-protect tab on SD card

**Test Write**:
```cpp
// Manual test in setup():
File test = SD.open("test.txt", FILE_WRITE);
test.println("Test");
test.close();
// Check if test.txt appears on SD card
```

### High Power Consumption

**Symptoms**: Battery drains faster than expected

**Causes & Solutions**:
- **Not entering standby**: Check USB connection (idle mode uses more power)
- **DEBUG_MODE enabled**: Set to `false` for production
- **LED always on**: Verify LED turns off after logging
- **SD card stays active**: Ensure `dataFile.close()` called
- **Short interval**: Increase SLEEP_INTERVAL

**Power Measurement**:
- Use multimeter in series with battery
- Expect ~6µA in standby (USB disconnected)
- Expect ~35mA peaks during logging

### Serial Monitor Issues

**Symptoms**: No output or garbled text

**Causes & Solutions**:
- **Wrong baud rate**: Set Serial Monitor to 9600 baud
- **COM port changed**: Verify correct port in Tools → Port
- **USB cable**: Use data cable, not charge-only
- **Buffer overflow**: Add delays if missing early messages

---

## Advanced Topics

### Customizing Sensor Readings

To add more sensors or modify readings, edit [SensorManager.cpp](main/SensorManager.cpp):

```cpp
SensorData SensorManager::readSensors()
{
    SensorData data;
    data.proximity = vcnl.readProximity();
    data.ambientLight = vcnl.readAmbient();
    
    // Add custom calculations:
    // data.turbidity = calculateTurbidity(data.proximity);
    
    return data;
}
```

Update [SensorManager.h](main/SensorManager.h) to add fields to `SensorData` struct.

### Adding Multiple Sensors

1. Add sensor library to includes
2. Create instance in SensorManager.h
3. Initialize in `begin()`
4. Read in `readSensors()`
5. Update SDLogger CSV format

Example for temperature sensor:
```cpp
// SensorManager.h
#include <Adafruit_DHT.h>
struct SensorData {
    uint16_t proximity;
    uint16_t ambientLight;
    float temperature;  // Added
    float humidity;     // Added
};

// SensorManager.cpp
bool SensorManager::begin() {
    if (!vcnl.begin()) return false;
    dht.begin();
    return true;
}

SensorData SensorManager::readSensors() {
    SensorData data;
    data.proximity = vcnl.readProximity();
    data.ambientLight = vcnl.readAmbient();
    data.temperature = dht.readTemperature();
    data.humidity = dht.readHumidity();
    return data;
}
```

### Remote Data Access

For wireless data transmission, consider:
1. **LoRa Module**: Long-range, low-power wireless
2. **WiFi Module**: Higher power, requires network
3. **Bluetooth**: Short-range data retrieval

Add wireless module to SAMD21 and transmit logged data periodically.

### Data Analysis

CSV file can be analyzed with:
- **Excel/Google Sheets**: Open CSV directly
- **Python**: pandas library for data analysis
- **R**: read.csv() for statistical analysis
- **MATLAB**: readtable() for signal processing

Example Python script:
```python
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('sensor.csv', parse_dates=['Timestamp'])
plt.plot(df['Timestamp'], df['Proximity'])
plt.xlabel('Time')
plt.ylabel('Turbidity (Proximity)')
plt.title('Water Turbidity Over Time')
plt.show()
```

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-12-16 | System | Initial comprehensive documentation |

---

## References

### Hardware Datasheets
- [SAMD21 Datasheet](https://www.microchip.com/en-us/product/ATSAMD21G18)
- [PCF8523 RTC Datasheet](https://www.nxp.com/docs/en/data-sheet/PCF8523.pdf)
- [VCNL4010 Sensor Datasheet](https://www.vishay.com/docs/83462/vcnl4010.pdf)

### Software Libraries
- [Adafruit RTClib](https://github.com/adafruit/RTClib)
- [Adafruit VCNL4010](https://github.com/adafruit/Adafruit_VCNL4010)
- [Adafruit SleepyDog](https://github.com/adafruit/Adafruit_SleepyDog_Library)

### Related Resources
- [Adafruit Feather M0 Adalogger Guide](https://learn.adafruit.com/adafruit-feather-m0-adalogger)
- [Low Power Arduino Techniques](https://www.gammon.com.au/power)
- [SAMD21 Standby Mode Guide](https://github.com/arduino/ArduinoCore-samd)

---

**End of Documentation**
