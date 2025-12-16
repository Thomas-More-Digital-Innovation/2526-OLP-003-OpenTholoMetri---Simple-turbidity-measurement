#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include <SD.h>
#include <RTClib.h>
#include "Config.h"
#include "SensorManager.h"

class SDLogger
{
public:
    SDLogger();

    // Initialize SD card and create file with headers if needed
    bool begin();

    // Log sensor data with timestamp to SD card
    bool logData(const DateTime &timestamp, const SensorData &data);

private:
    const char *fileName;

    // Format timestamp as string
    void formatTimestamp(const DateTime &timestamp, char *buffer);
};

#endif // SD_LOGGER_H
