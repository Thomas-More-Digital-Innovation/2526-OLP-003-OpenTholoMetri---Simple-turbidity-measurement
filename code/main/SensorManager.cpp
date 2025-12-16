#include "SensorManager.h"

SensorManager::SensorManager()
{
}

bool SensorManager::begin()
{
    // Initialize VCNL4010 sensor
    if (!vcnl.begin())
    {
        Serial.println("VCNL4010 sensor initialization failed!");
        return false;
    }

    Serial.println("VCNL4010 sensor initialized successfully!");
    return true;
}

SensorData SensorManager::readSensors()
{
    SensorData data;
    data.proximity = vcnl.readProximity();
    data.ambientLight = vcnl.readAmbient();
    return data;
}

SensorData SensorManager::readAveragedSensors(int count, int intervalMs)
{
    uint32_t proximitySum = 0;
    uint32_t ambientLightSum = 0;

    // Take multiple measurements
    for (int i = 0; i < count; i++)
    {
        SensorData sample = readSensors();
        proximitySum += sample.proximity;
        ambientLightSum += sample.ambientLight;

        Serial.println("Measurement " + String(i) + ": Proximity=" + String(sample.proximity) + ", AmbientLight=" + String(sample.ambientLight));

        // Delay between measurements (skip delay after last measurement)
        if (i < count - 1)
        {
            delay(intervalMs);
        }
    }

    // Calculate averages
    SensorData averaged;
    averaged.proximity = proximitySum / count;
    averaged.ambientLight = ambientLightSum / count;

    return averaged;
}
