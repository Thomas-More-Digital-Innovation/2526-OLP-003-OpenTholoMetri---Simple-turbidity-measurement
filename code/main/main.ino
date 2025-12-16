#include "Config.h"
#include "RTCManager.h"
#include "SensorManager.h"
#include "SDLogger.h"

// Component instances
RTCManager rtcManager;
SensorManager sensorManager;
SDLogger sdLogger;

// RTC interrupt flag
volatile bool rtcInterruptFired = false;

// RTC interrupt handler
void rtcInterruptHandler()
{
  rtcInterruptFired = true;
}

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
    while (true);
  }

  // Setup RTC timer and interrupt
  rtcManager.setupTimer();

  // Configure RTC interrupt pin
  pinMode(RTC_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RTC_INTERRUPT_PIN), rtcInterruptHandler, FALLING);
  
  // Configure clocks for standby wake capability
  // Set external 32k oscillator to run when in standby mode
  SYSCTRL->XOSC32K.reg |= (SYSCTRL_XOSC32K_RUNSTDBY | SYSCTRL_XOSC32K_ONDEMAND);
  
  // Configure generic clock for the External Interrupt Controller (EIC)
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID(GCM_EIC) |      // Generic clock for EIC
                     GCLK_CLKCTRL_GEN_GCLK1 |         // Use GCLK1 (32kHz oscillator)
                     GCLK_CLKCTRL_CLKEN;              // Enable it
  while (GCLK->STATUS.bit.SYNCBUSY);                  // Wait for sync
  
  // Enable wakeup from standby mode for RTC interrupt on pin 5
  // Pin 5 corresponds to EIC channel 5 (EXTINT[5])
  EIC->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN5;

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
  if (!rtcInterruptFired)
  {
    // If USB is connected (Serial available), use lighter sleep to maintain serial connection
    if (Serial)
    {
      // Use Idle mode instead of Standby - keeps USB alive for debugging
      // This uses more power (~15mA) but allows serial monitoring
      __DSB();
      __WFI();
    }
    else
    {
      // Enter deep sleep (standby mode) - will wake on RTC interrupt on pin 5
      // SAMD21 standby mode uses ~6µA and can wake from external interrupts
      SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; // Enable deep sleep
      PM->SLEEP.reg = PM_SLEEP_IDLE(2); // Set standby mode (IDLE=2)
      __DSB(); // Data Synchronization Barrier
      __WFI(); // Wait For Interrupt - enters standby mode
    }
    return;
  }

  // If we get here, the interrupt fired!
  Serial.println("*** RTC INTERRUPT FIRED! **!");

  // RTC interrupt was triggered - countdown reached 0 and auto-restarted
  // Reset the interrupt flag for next cycle
  rtcInterruptFired = false;

  Serial.println("Reading sensor data...");
  // Read multiple measurements and average them
  SensorData sensorData = sensorManager.readAveragedSensors(MEASUREMENT_COUNT, MEASUREMENT_INTERVAL_MS);

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
