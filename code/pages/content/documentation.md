# Documentation

## Hardware Components

- **Microcontroller**: Arduino-compatible board
- **RTC Module**: Real-time clock for accurate timestamps
- **SD Card Module**: Data storage
- **Turbidity Sensor**: Grove Turbidity Sensor
- **Temperature Sensor**: DS18B20
- **Power Management**: Battery-powered with voltage monitoring

## Firmware

The firmware is located in the `code/main/` directory and includes:

- `main.ino` - Main program logic
- `RTCManager` - Real-time clock management
- `SDLogger` - SD card logging functionality
- `SensorManager` - Sensor reading and management

## CSV Data Format

The device logs data in CSV format with the following structure:

```
Timestamp,Proximity,Ambient Light,Battery Voltage
2024-01-15 10:30:00,245,1023,4.15
2024-01-15 10:35:00,248,1019,4.14
```

### Column Descriptions

- **Timestamp**: Local time in YYYY-MM-DD HH:MM:SS format
- **Proximity**: Turbidity sensor proximity value (0-1023)
- **Ambient Light**: Ambient light sensor value (0-1023)
- **Battery Voltage**: Battery voltage in volts

## Build Instructions

For detailed build instructions, see the project repository.
