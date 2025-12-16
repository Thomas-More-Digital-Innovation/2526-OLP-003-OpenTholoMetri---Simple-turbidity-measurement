#include <Adafruit_SleepyDog.h>
#include "Config.h"
#include "RTCManager.h"
#include "SensorManager.h"
#include "SDLogger.h"

// Component instances
RTCManager rtcManager;
SensorManager sensorManager;
SDLogger sdLogger;

void setup()
{
  Serial.begin(9600);
  delay(2000); // Wait for serial monitor

  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Initialize RTC
  if (!rtcManager.begin())
  {
    while (true)
      ;
  }

  // Setup RTC timer and interrupt
  rtcManager.setupTimer();

  // Initialize SD card
  if (!sdLogger.begin())
  {
    while (true)
      ;
  }

  // Initialize sensor
  if (!sensorManager.begin())
  {
    while (true)
      ;
  }

  Serial.println("System initialized successfully!");
  Serial.println("Waiting for first RTC interrupt...");
  Serial.flush();
}

void loop()
{
  // Check if RTC interrupt fired
  if (!RTCManager::isInterruptFired())
  {
    // No interrupt yet, just wait in low power mode
    // On SAMD21 (Feather M0), external interrupts work differently with sleep
    // For now, use a short delay to reduce power while waiting
    delay(100);
    return;
  }

  // If we get here, the interrupt fired!
  Serial.println("*** RTC INTERRUPT FIRED! ***");

  // RTC interrupt was triggered - countdown reached 0 and auto-restarted
  // Reset the interrupt flag for next cycle
  RTCManager::resetInterruptFlag();

  Serial.println("Reading sensor data...");
  // Read sensor data
  SensorData sensorData = sensorManager.readSensors();

  // Get current timestamp
  DateTime timestamp = rtcManager.now();

  // Flash LED to indicate logging
  if (DEBUG_MODE)
  {
    digitalWrite(LED_PIN, HIGH);
  }

  // Log to SD card
  sdLogger.logData(timestamp, sensorData);

  // Turn off LED after logging
  if (DEBUG_MODE)
  {
    digitalWrite(LED_PIN, LOW);
  }

  if (DEBUG_MODE)
  {
    Serial.println("Data logged. Going to sleep...");
    Serial.flush();
  }

  // Sleep will be handled at the beginning of next loop iteration
  // The countdown timer is already running again automatically
}
