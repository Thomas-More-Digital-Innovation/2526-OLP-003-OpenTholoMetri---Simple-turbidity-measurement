#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Adafruit_VCNL4010.h>
#include <OneWire.h>
#include <DallasTemperature.h>

struct SensorData
{
    uint16_t proximity;
    uint16_t ambientLight;
    float batteryVoltage;
    float temperatureC;        // DS18B20 temperature in Celsius
    int turbidityRaw;          // Grove turbidity sensor raw ADC value
    float turbidityVoltage;    // Grove turbidity sensor voltage
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
    OneWire oneWire;
    DallasTemperature ds18b20;
};

#endif // SENSOR_MANAGER_H
