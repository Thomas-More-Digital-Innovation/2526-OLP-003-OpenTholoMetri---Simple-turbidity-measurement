/* DS18B20 1-Wire digital temperature sensor with Arduino example code. 
   https://www.makerguides.com */

// Include the required Arduino libraries:
#include "OneWire.h"
#include "DallasTemperature.h"

// Define to which pin of the Arduino the 1-Wire bus is connected:
#define ONE_WIRE_BUS A0

// Power control pin for the DS18B20 VCC
const int SENSOR_POWER_PIN = 12; // connect DS18B20 Vcc to digital pin 12
const unsigned long POWER_STABILIZATION_MS = 1000; // ms to wait after powering sensor

// Create a new instance of the oneWire class to communicate with any OneWire device:
OneWire oneWire(ONE_WIRE_BUS);

// Pass the oneWire reference to DallasTemperature library:
DallasTemperature sensors(&oneWire);

void setup() {
  // Begin serial communication at a baud rate of 9600:
  Serial.begin(9600);

  // Configure sensor power pin and ensure sensor is off initially
  pinMode(SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN, LOW);

  // Temporarily power sensor to initialize the Dallas library, then power off
  digitalWrite(SENSOR_POWER_PIN, HIGH);
  delay(POWER_STABILIZATION_MS);
  sensors.begin();
  digitalWrite(SENSOR_POWER_PIN, LOW);

  // Ensure data pin has pull-up
  pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
}

void loop() {
  // Power the sensor
  digitalWrite(SENSOR_POWER_PIN, HIGH);

  // Allow sensor to stabilize after power-up
  delay(POWER_STABILIZATION_MS);

  // Send the command for all devices on the bus to perform a temperature conversion (blocking)
  sensors.requestTemperatures();

  // Fetch the temperature in degrees Celsius for device index:
  float tempC = sensors.getTempCByIndex(0); // the index 0 refers to the first device
  // Fetch the temperature in degrees Fahrenheit for device index:
  float tempF = sensors.getTempFByIndex(0);

  // Print the temperature in Celsius in the Serial Monitor:
  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.print(" \xC2\xB0"); // shows degree symbol
  Serial.print("C  |  ");

  // Print the temperature in Fahrenheit
  Serial.print(tempF);
  Serial.print(" \xC2\xB0"); // shows degree symbol
  Serial.println("F");

  // Turn sensor power off to save energy
  digitalWrite(SENSOR_POWER_PIN, LOW);

  // Wait 1 second until next measurement
  delay(1000);
}