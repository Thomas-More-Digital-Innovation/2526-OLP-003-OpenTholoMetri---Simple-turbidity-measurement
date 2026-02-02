#include "SDLogger.h"

SDLogger::SDLogger() : fileName(LOG_FILE_NAME)
{
}

bool SDLogger::begin()
{
    sdInitialized = initializeCardAndEnsureFile();
    return sdInitialized;
}

bool SDLogger::initializeCardAndEnsureFile()
{
    // Re-(initialize) SD card. After removal/reinsert, the SD stack often needs SD.begin() again.
    if (!SD.begin(SD_CS_PIN))
    {
        Serial.println("SD card initialization failed!");
        return false;
    }

    // Create CSV file with headers if it doesn't exist
    if (!SD.exists(fileName))
    {
        File dataFile = SD.open(fileName, FILE_WRITE);
        if (!dataFile)
        {
            Serial.println("Failed to create log file!");
            return false;
        }

        dataFile.println("Timestamp,Proximity,Ambient Light,Battery Voltage,Temperature (C),Turbidity Raw,Turbidity Voltage");
        dataFile.close();
        Serial.println("Created new log file with headers");
    }

    Serial.println("SD card initialized successfully!");
    return true;
}

bool SDLogger::logData(const DateTime &timestamp, const SensorData &data)
{
    if (!sdInitialized)
    {
        sdInitialized = initializeCardAndEnsureFile();
        if (!sdInitialized)
        {
            Serial.println("Error opening log file!");
            return false;
        }
    }

    File dataFile = SD.open(fileName, FILE_WRITE);
    if (!dataFile)
    {
        // If the SD was removed and reinserted, SD.open() can keep failing until we re-run SD.begin().
        sdInitialized = false;
        sdInitialized = initializeCardAndEnsureFile();
        if (sdInitialized)
        {
            dataFile = SD.open(fileName, FILE_WRITE);
        }

        if (!dataFile)
        {
            Serial.println("Error opening log file!");
            return false;
        }
    }

    char timestampStr[20];
    formatTimestamp(timestamp, timestampStr);

    dataFile.print(timestampStr);
    dataFile.print(",");
    dataFile.print(data.proximity);
    dataFile.print(",");
    dataFile.print(data.ambientLight);
    dataFile.print(",");
    dataFile.print(data.batteryVoltage, 2);
    dataFile.print(",");
    dataFile.print(data.temperatureC, 2);
    dataFile.print(",");
    dataFile.print(data.turbidityRaw);
    dataFile.print(",");
    dataFile.println(data.turbidityVoltage, 4);
    dataFile.close();

    Serial.print("Logged: ");
    Serial.print(timestampStr);
    Serial.print(" | Prox: ");
    Serial.print(data.proximity);
    Serial.print(" | Light: ");
    Serial.print(data.ambientLight);
    Serial.print(" | Battery: ");
    Serial.print(data.batteryVoltage, 2);
    Serial.print("V | Temp: ");
    Serial.print(data.temperatureC, 2);
    Serial.print("C | Turbidity: ");
    Serial.print(data.turbidityRaw);
    Serial.print(" (");
    Serial.print(data.turbidityVoltage, 4);
    Serial.println("V)");

    return true;
}

void SDLogger::formatTimestamp(const DateTime &timestamp, char *buffer)
{
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
            timestamp.year(), timestamp.month(), timestamp.day(),
            timestamp.hour(), timestamp.minute(), timestamp.second());
}
