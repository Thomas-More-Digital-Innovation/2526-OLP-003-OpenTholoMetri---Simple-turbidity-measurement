#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions
const int SD_CS_PIN = 10;        // SD card CS pin on Adalogger
const int LED_PIN = 13;          // LED pin
const int RTC_INTERRUPT_PIN = 5; // RTC interrupt pin (SQW/INT on Adalogger)

// Timing configuration
const int SLEEP_INTERVAL = 10; // seconds between logs

// Measurement configuration
const int MEASUREMENT_COUNT = 10;        // Number of measurements to average
const int MEASUREMENT_INTERVAL_MS = 100; // Milliseconds between measurements

// Debug configuration
const bool DEBUG_MODE = true;

// File configuration
static const char *LOG_FILE_NAME = "sensor.csv";

#endif // CONFIG_H
