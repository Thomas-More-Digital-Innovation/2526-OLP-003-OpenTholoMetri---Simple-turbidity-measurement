#include "RTCManager.h"
#include <Wire.h>

RTCManager::RTCManager()
{
}

bool RTCManager::begin()
{
    // Initialize RTC on Adalogger
    if (!rtc.begin())
    {
        Serial.println("RTC initialization failed!");
        return false;
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

    Serial.println("RTC initialized successfully!");
    return true;
}

void RTCManager::setupTimer()
{
    // Timer configuration is not cleared on RTC reset due to battery backup!
    // Always deconfigure all timers before setting up a new one
    rtc.deconfigureAllTimers();

    // Enable countdown timer - it will automatically restart after each countdown!
    // The PCF8523 will pull the INT pin LOW briefly when countdown reaches 0,
    // then immediately start counting down again.
    rtc.enableCountdownTimer(PCF8523_FrequencySecond, SLEEP_INTERVAL, PCF8523_LowPulse3x64Hz);

    Serial.print("RTC countdown timer enabled for ");
    Serial.print(SLEEP_INTERVAL);
    Serial.println(" second intervals");
    Serial.println("Note: First countdown period may be slightly inaccurate.");
}

DateTime RTCManager::now()
{
    return rtc.now();
}
