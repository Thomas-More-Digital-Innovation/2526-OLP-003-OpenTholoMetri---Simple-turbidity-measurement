void setup() {
  // Initialize serial for debug output
  Serial.begin(9600); // Baud rate: 9600

  // Configure ADC for SAMD21 (M0) to 12-bit resolution (0-4095)
  // and use the 3.3V reference typical for M0 boards.
  analogReadResolution(12);
}

void loop() {
  // Read the analog value from A0 on the M0
  int sensorValue = analogRead(A0); // read the input on analog pin A0

  // Convert the analog reading to voltage using 3.3V and 12-bit resolution
  float voltage = sensorValue * (3.3 / 4095.0);

  // Print the voltage with 4 decimal places
  Serial.println(voltage, 4);

  delay(500);
}
