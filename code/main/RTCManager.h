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

private:
    RTC_PCF8523 rtc;
};

#endif // RTC_MANAGER_H
