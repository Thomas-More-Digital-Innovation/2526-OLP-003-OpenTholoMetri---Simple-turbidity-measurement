// Pin assignments
const int SENSOR_POWER_PIN = 11; // VCC for Grove sensor connected to digital pin 11
const int SENSOR_ANALOG_PIN = A1; // Analog reading on A1
const unsigned long POWER_STABILIZATION_MS = 20; // Time to wait after powering sensor (ms)

void setup() {
  // Initialize serial for debug output
  Serial.begin(9600); // Baud rate: 9600

  // Configure ADC for SAMD21 (M0) to 12-bit resolution (0-4095)
  // and use the 3.3V reference typical for M0 boards.
  analogReadResolution(12);

  // Configure sensor power pin and ensure sensor is off initially
  pinMode(SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN, LOW);
}

void loop() {
  // Power the sensor
  digitalWrite(SENSOR_POWER_PIN, HIGH);

  // Allow sensor to stabilize after power-up
  delay(POWER_STABILIZATION_MS);

  // Read the analog value from A1 on the M0
  int sensorValue = analogRead(SENSOR_ANALOG_PIN); // raw ADC value (0-4095)

  // Convert the analog reading to voltage using 3.3V and 12-bit resolution
  float voltage = sensorValue * (3.3 / 4095.0);

  // Print raw and voltage values
  Serial.print("Raw: ");
  Serial.print(sensorValue);
  Serial.print("  V: ");
  Serial.println(voltage, 4);

  // Turn sensor power off to save energy
  digitalWrite(SENSOR_POWER_PIN, LOW);

  // Wait until next measurement
  delay(500);
}
