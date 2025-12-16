#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Adafruit_VCNL4010.h>

struct SensorData
{
    uint16_t proximity;
    uint16_t ambientLight;
    float batteryVoltage;
};

class SensorManager
{
public:
    SensorManager();

    // Initialize the sensor
    bool begin();

    // Read sensor data
    SensorData readSensors();

    // Read multiple sensor measurements and return averaged values
    SensorData readAveragedSensors(int count, int intervalMs);

private:
    Adafruit_VCNL4010 vcnl;
};

#endif // SENSOR_MANAGER_H
