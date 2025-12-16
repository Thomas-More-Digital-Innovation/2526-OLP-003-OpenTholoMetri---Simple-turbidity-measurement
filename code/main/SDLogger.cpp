#include "SDLogger.h"

SDLogger::SDLogger() : fileName(LOG_FILE_NAME)
{
}

bool SDLogger::begin()
{
    // Initialize SD card
    if (!SD.begin(SD_CS_PIN))
    {
        Serial.println("SD card initialization failed!");
        return false;
    }

    // Create CSV file with headers if it doesn't exist
    if (!SD.exists(fileName))
    {
        File dataFile = SD.open(fileName, FILE_WRITE);
        if (dataFile)
        {
            dataFile.println("Timestamp,Proximity,Ambient Light");
            dataFile.close();
            Serial.println("Created new log file with headers");
        }
        else
        {
            Serial.println("Failed to create log file!");
            return false;
        }
    }

    Serial.println("SD card initialized successfully!");
    return true;
}

bool SDLogger::logData(const DateTime &timestamp, const SensorData &data)
{
    File dataFile = SD.open(fileName, FILE_WRITE);
    if (!dataFile)
    {
        Serial.println("Error opening log file!");
        return false;
    }

    char timestampStr[20];
    formatTimestamp(timestamp, timestampStr);

    dataFile.print(timestampStr);
    dataFile.print(",");
    dataFile.print(data.proximity);
    dataFile.print(",");
    dataFile.println(data.ambientLight);
    dataFile.close();

    Serial.print("Logged: ");
    Serial.print(timestampStr);
    Serial.print(" | Prox: ");
    Serial.print(data.proximity);
    Serial.print(" | Light: ");
    Serial.println(data.ambientLight);

    return true;
}

void SDLogger::formatTimestamp(const DateTime &timestamp, char *buffer)
{
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
            timestamp.year(), timestamp.month(), timestamp.day(),
            timestamp.hour(), timestamp.minute(), timestamp.second());
}
