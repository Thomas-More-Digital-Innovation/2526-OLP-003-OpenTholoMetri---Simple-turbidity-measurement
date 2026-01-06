#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions
const int SD_CS_PIN = 10;        // SD card CS pin on Adalogger
const int LED_PIN = 13;          // LED pin
const int RTC_INTERRUPT_PIN = 5; // RTC interrupt pin (SQW/INT on Adalogger)
const int BATTERY_PIN = 9;       // Battery voltage pin

// Timing configuration
const int SLEEP_INTERVAL = 10; // seconds between logs

// Measurement configuration
const int MEASUREMENT_COUNT = 10;        // Number of measurements to average
const int MEASUREMENT_INTERVAL_MS = 100; // Milliseconds between measurements

// VCNL4010 configuration (match OpenOBS defaults)
// LED current is in 10mA units: 5 => 50mA
const uint8_t VCNL4010_LED_CURRENT_10MA = 5;
// Proximity measurement rate (matches Adafruit VCNL4010 enum values):
// 0..7 => {1.95, 3.90625, 7.8125, 16.625, 31.25, 62.5, 125, 250} measurements/sec
// OpenOBS default is 250 => 7
const uint8_t VCNL4010_PROX_RATE_VALUE = 7;
// Ambient averaging is log2 of number of conversions: 0=>1, 1=>2, ... 7=>128
const uint8_t VCNL4010_AMBIENT_AVERAGING_LOG2 = 1;
// Continuous ambient conversions (bit 7 in VCNL4010_AMBIENTPARAMETER)
const bool VCNL4010_AMBIENT_CONTINUOUS = true;

// Debug configuration
const bool DEBUG_MODE = true;

// File configuration
static const char *LOG_FILE_NAME = "sensor.csv";

#endif // CONFIG_H
