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
