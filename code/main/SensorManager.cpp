#include "SensorManager.h"
#include "Config.h"

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

    // Read battery voltage from analog pin
    // Battery voltage goes through a voltage divider (/2), so multiply back
    float measuredvbat = analogRead(BATTERY_PIN);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    data.batteryVoltage = measuredvbat;

    return data;
}

SensorData SensorManager::readAveragedSensors(int count, int intervalMs)
{
    uint32_t proximitySum = 0;
    uint32_t ambientLightSum = 0;
    float batteryVoltageSum = 0;

    // Take multiple measurements
    for (int i = 0; i < count; i++)
    {
        SensorData sample = readSensors();
        proximitySum += sample.proximity;
        ambientLightSum += sample.ambientLight;
        batteryVoltageSum += sample.batteryVoltage;

        Serial.println("Measurement " + String(i) + ": Proximity=" + String(sample.proximity) + ", AmbientLight=" + String(sample.ambientLight) + ", BatteryV=" + String(sample.batteryVoltage, 2));

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
    averaged.batteryVoltage = batteryVoltageSum / count;

    return averaged;
}
