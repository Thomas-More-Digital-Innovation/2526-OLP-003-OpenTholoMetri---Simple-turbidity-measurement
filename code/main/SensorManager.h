#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Adafruit_VCNL4010.h>

struct SensorData
{
    uint16_t proximity;
    uint16_t ambientLight;
};

class SensorManager
{
public:
    SensorManager();

    // Initialize the sensor
    bool begin();

    // Read sensor data
    SensorData readSensors();

private:
    Adafruit_VCNL4010 vcnl;
};

#endif // SENSOR_MANAGER_H
