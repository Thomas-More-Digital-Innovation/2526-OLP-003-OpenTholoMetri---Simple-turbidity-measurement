#include "SensorManager.h"
#include "Config.h"

#include <Wire.h>

namespace
{
    bool vcnl4010Write8(uint8_t reg, uint8_t value)
    {
        Wire.beginTransmission(VCNL4010_I2CADDR_DEFAULT);
        Wire.write(reg);
        Wire.write(value);
        return Wire.endTransmission() == 0;
    }

    bool vcnl4010Read8(uint8_t reg, uint8_t &value)
    {
        Wire.beginTransmission(VCNL4010_I2CADDR_DEFAULT);
        Wire.write(reg);
        if (Wire.endTransmission(false) != 0)
        {
            return false;
        }

        uint8_t readCount = Wire.requestFrom((int)VCNL4010_I2CADDR_DEFAULT, 1);
        if (readCount != 1 || !Wire.available())
        {
            return false;
        }

        value = Wire.read();
        return true;
    }

    bool vcnl4010SetAmbientAveraging(uint8_t log2Avg)
    {
        if (log2Avg > 7)
        {
            log2Avg = 7;
        }

        uint8_t reg = 0;
        if (!vcnl4010Read8(VCNL4010_AMBIENTPARAMETER, reg))
        {
            return false;
        }

        reg &= 0b11111000;
        reg |= (log2Avg & 0b00000111);
        return vcnl4010Write8(VCNL4010_AMBIENTPARAMETER, reg);
    }

    bool vcnl4010SetAmbientContinuous(bool enable)
    {
        uint8_t reg = 0;
        if (!vcnl4010Read8(VCNL4010_AMBIENTPARAMETER, reg))
        {
            return false;
        }

        reg = (reg & 0b01111111) | ((enable ? 1 : 0) << 7);
        return vcnl4010Write8(VCNL4010_AMBIENTPARAMETER, reg);
    }
}

SensorManager::SensorManager()
    : oneWire(DS18B20_DATA_PIN), ds18b20(&oneWire)
{
}

bool SensorManager::begin()
{
    // Configure ADC for SAMD21 (M0) to 12-bit resolution (0-4095)
    analogReadResolution(12);

    // Initialize DS18B20 power pin
    pinMode(DS18B20_POWER_PIN, OUTPUT);
    digitalWrite(DS18B20_POWER_PIN, LOW);
    pinMode(DS18B20_DATA_PIN, INPUT_PULLUP);

    // Initialize Grove turbidity sensor power pin
    pinMode(GROVE_POWER_PIN, OUTPUT);
    digitalWrite(GROVE_POWER_PIN, LOW);

    // Temporarily power DS18B20 to initialize the Dallas library
    digitalWrite(DS18B20_POWER_PIN, HIGH);
    delay(DS18B20_POWER_STABILIZATION_MS);
    ds18b20.begin();
    digitalWrite(DS18B20_POWER_PIN, LOW);

    Serial.println("DS18B20 sensor initialized successfully!");
    Serial.println("Grove turbidity sensor initialized successfully!");

    // Initialize VCNL4010 sensor
    if (!vcnl.begin())
    {
        Serial.println("VCNL4010 sensor initialization failed!");
        return false;
    }

    // Apply OpenOBS-like defaults (Adafruit begin() sets its own defaults; override them here)
    vcnl.setLEDcurrent(VCNL4010_LED_CURRENT_10MA);
    vcnl.setFrequency((vcnl4010_freq)VCNL4010_PROX_RATE_VALUE);

    bool ambientOk = vcnl4010SetAmbientAveraging(VCNL4010_AMBIENT_AVERAGING_LOG2) &&
                     vcnl4010SetAmbientContinuous(VCNL4010_AMBIENT_CONTINUOUS);

    if (DEBUG_MODE)
    {
        Serial.println("VCNL4010 settings applied: LEDcurrent=" + String(VCNL4010_LED_CURRENT_10MA) + " (x10mA), ProxRate=" + String(VCNL4010_PROX_RATE_VALUE) + ", AmbientAvgLog2=" + String(VCNL4010_AMBIENT_AVERAGING_LOG2) + ", AmbientContinuous=" + String(VCNL4010_AMBIENT_CONTINUOUS ? "true" : "false"));
        if (!ambientOk)
        {
            Serial.println("Warning: failed to apply VCNL4010 ambient settings via I2C register write.");
        }
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

    // Read DS18B20 temperature sensor
    digitalWrite(DS18B20_POWER_PIN, HIGH);
    delay(DS18B20_POWER_STABILIZATION_MS);
    ds18b20.requestTemperatures();
    data.temperatureC = ds18b20.getTempCByIndex(0);
    digitalWrite(DS18B20_POWER_PIN, LOW);

    // Read Grove turbidity sensor
    digitalWrite(GROVE_POWER_PIN, HIGH);
    delay(GROVE_POWER_STABILIZATION_MS);
    data.turbidityRaw = analogRead(GROVE_ANALOG_PIN);
    data.turbidityVoltage = data.turbidityRaw * (3.3 / 4095.0);
    digitalWrite(GROVE_POWER_PIN, LOW);

    return data;
}

SensorData SensorManager::readAveragedSensors(int count, int intervalMs)
{
    uint32_t proximitySum = 0;
    uint32_t ambientLightSum = 0;
    float batteryVoltageSum = 0;
    float temperatureSum = 0;
    uint32_t turbidityRawSum = 0;
    float turbidityVoltageSum = 0;

    // Take multiple measurements
    for (int i = 0; i < count; i++)
    {
        SensorData sample = readSensors();
        proximitySum += sample.proximity;
        ambientLightSum += sample.ambientLight;
        batteryVoltageSum += sample.batteryVoltage;
        temperatureSum += sample.temperatureC;
        turbidityRawSum += sample.turbidityRaw;
        turbidityVoltageSum += sample.turbidityVoltage;

        Serial.println("Measurement " + String(i) + ": Proximity=" + String(sample.proximity) + ", AmbientLight=" + String(sample.ambientLight) + ", BatteryV=" + String(sample.batteryVoltage, 2) + ", TempC=" + String(sample.temperatureC, 2) + ", TurbidityRaw=" + String(sample.turbidityRaw) + ", TurbidityV=" + String(sample.turbidityVoltage, 4));

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
    averaged.temperatureC = temperatureSum / count;
    averaged.turbidityRaw = turbidityRawSum / count;
    averaged.turbidityVoltage = turbidityVoltageSum / count;

    return averaged;
}
