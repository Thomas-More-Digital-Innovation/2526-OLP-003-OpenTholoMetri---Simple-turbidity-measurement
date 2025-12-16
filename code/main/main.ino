#include <RTClib.h>
#include <SD.h>
#include <Adafruit_VCNL4010.h>
#include <Adafruit_SleepyDog.h>

const int sdCS = 10;           // SD card CS pin on Adalogger
const int ledPin = 13;         // LED pin
const int rtcInterruptPin = 6; // RTC interrupt pin (SQW/INT on Adalogger)
const int SLEEP_INTERVAL = 60; // seconds between logs
const bool debug = true;

Adafruit_VCNL4010 vcnl;
RTC_PCF8523 rtc; // Built-in RTC on Adalogger
const char *logFileName = "sensor.csv";

volatile bool rtcInterruptFired = false;

// RTC interrupt handler
void rtcInterruptHandler()
{
  rtcInterruptFired = true;
}

// Function to manually clear the PCF8523 timer flag in Control_2 register
void clearRTCTimerFlag()
{
  // PCF8523 Control_2 register address is 0x01
  // CTBF (Countdown Timer B Flag) is bit 0
  // Write 0 to clear the interrupt flag
  Wire.beginTransmission(0x68); // PCF8523 I2C address
  Wire.write(0x01);             // Control_2 register
  Wire.write(0x00);             // Clear all flags including CTBF
  Wire.endTransmission();
}

void setup()
{
  Serial.begin(9600);
  delay(2000); // Wait for serial monitor

  // Initialize LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Initialize RTC interrupt pin with pullup (INT is open-drain)
  pinMode(rtcInterruptPin, INPUT_PULLUP);

  // Initialize RTC on Adalogger
  if (!rtc.begin())
  {
    Serial.println("RTC initialization failed!");
    while (true)
      ;
  }

  // If RTC lost power, set the time
  if (!rtc.initialized() || rtc.lostPower())
  {
    Serial.println("RTC not initialized or lost power, setting time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  // IMPORTANT: Without a battery, we need to ensure the RTC is in the right mode
  // The adjust() call above should set battery switchover mode, but let's ensure it
  // Set the time again to force proper initialization even without battery
  rtc.adjust(rtc.now());

  // Start the RTC
  rtc.start();

  // Stop all timers and clear flags
  rtc.deconfigureAllTimers();
  delay(50);

  // Manually clear any timer flags via I2C
  clearRTCTimerFlag();
  delay(50);

  // Attach interrupt handler BEFORE enabling timer
  attachInterrupt(digitalPinToInterrupt(rtcInterruptPin), rtcInterruptHandler, FALLING);

  // Enable countdown timer for low-power operation
  rtc.enableCountdownTimer(PCF8523_FrequencySecond, SLEEP_INTERVAL, PCF8523_LowPulse3x64Hz);

  Serial.print("RTC countdown timer enabled for ");
  Serial.print(SLEEP_INTERVAL);
  Serial.println(" second intervals");
  // Initialize SD card
  if (!SD.begin(sdCS))
  {
    Serial.println("SD card initialization failed!");
    while (true)
      ;
  }

  // Initialize VCNL4010 sensor
  if (!vcnl.begin())
  {
    Serial.println("VCNL4010 sensor initialization failed!");
    while (true)
      ;
  }

  // Create CSV file with headers if it doesn't exist
  if (!SD.exists(logFileName))
  {
    File dataFile = SD.open(logFileName, FILE_WRITE);
    if (dataFile)
    {
      dataFile.println("Timestamp,Proximity,Ambient Light");
      dataFile.close();
    }
  }

  Serial.println("System initialized successfully!");
}

void loop()
{
  // Check if RTC interrupt fired
  if (!rtcInterruptFired)
  {
    // Go to deep sleep and wait for RTC interrupt
    Watchdog.sleep();
    return;
  }

  // Reset the interrupt flag
  rtcInterruptFired = false;

  // Manually clear the RTC timer flag via direct I2C write
  clearRTCTimerFlag();
  delay(50);

  // Re-enable the countdown timer for the next cycle
  rtc.enableCountdownTimer(PCF8523_FrequencySecond, SLEEP_INTERVAL, PCF8523_LowPulse3x64Hz);

  // Read sensor data
  uint16_t proximity = vcnl.readProximity();
  uint16_t ambient = vcnl.readAmbient();

  // Get current timestamp
  DateTime now = rtc.now();

  // Flash LED to indicate logging
  if (debug)
  {
    digitalWrite(ledPin, HIGH);
  }

  // Log to SD card
  File dataFile = SD.open(logFileName, FILE_WRITE);
  if (dataFile)
  {
    char timestamp[20];
    sprintf(timestamp, "%04d-%02d-%02d %02d:%02d:%02d",
            now.year(), now.month(), now.day(),
            now.hour(), now.minute(), now.second());

    dataFile.print(timestamp);
    dataFile.print(",");
    dataFile.print(proximity);
    dataFile.print(",");
    dataFile.println(ambient);
    dataFile.close();

    Serial.print("Logged: ");
    Serial.print(timestamp);
    Serial.print(" | Prox: ");
    Serial.print(proximity);
    Serial.print(" | Light: ");
    Serial.println(ambient);
  }
  else
  {
    Serial.println("Error opening log file!");
  }

  // Turn off LED after logging
  if (debug)
  {
    digitalWrite(ledPin, LOW);
  }

  Serial.println("Going to sleep...");
  Serial.flush();

  // Sleep will be handled at the beginning of next loop iteration
  // The RTC interrupt will wake the microcontroller precisely
}
