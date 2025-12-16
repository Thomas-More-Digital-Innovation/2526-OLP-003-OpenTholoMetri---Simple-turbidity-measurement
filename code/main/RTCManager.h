#ifndef RTC_MANAGER_H
#define RTC_MANAGER_H

#include <RTClib.h>
#include "Config.h"

class RTCManager
{
public:
    RTCManager();

    // Initialize the RTC
    bool begin();

    // Setup countdown timer and interrupt
    void setupTimer();

    // Get current time
    DateTime now();

    // Check if interrupt fired
    static bool isInterruptFired();

    // Reset interrupt flag
    static void resetInterruptFlag();

    // Interrupt handler (must be static for attachInterrupt)
    static void interruptHandler();

private:
    RTC_PCF8523 rtc;
    static volatile bool interruptFired;
};

#endif // RTC_MANAGER_H
