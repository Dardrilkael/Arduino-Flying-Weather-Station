#pragma once
#include <Arduino.h>
#include "MQ7.h"

class MQ4GasSensor {
public:
  MQ4GasSensor(uint8_t analogPin, float rl, float ro, float a, float b)
      : pin(analogPin), RL(rl), R0(ro), A(a), B(b) {}

  // Function to read the Rs value
  float readRs() {
    int adcValue = analogRead(pin);  // Read analog pin value
    float voltage = adcValue * VREF / 1023.0;  // Convert ADC to voltage
    return (RL * (VREF - voltage)) / voltage;  // Calculate Rs
  }

  // Function to calculate the PPM (Parts Per Mil   lion) based on the formula
  float getPPM() {
    float Rs = readRs();  // Get Rs value
    float ratio = Rs / R0;  // Calculate the ratio of Rs/R0
    return A * pow(ratio, B);  // Calculate PPM using the power law formula
  }

  // Function to get the ratio of Rs to R0
  float getRatio() {
    return readRs() / R0;
  }

private:
  const float VREF = 5.0;  // Reference voltage (adjust based on your ESP32 or Arduino board)
  uint8_t pin;  // Analog pin connected to the sensor
  float RL;     // Load resistance (kΩ)
  float R0;     // Resistance in clean air (kΩ)
  float A;      // Calibration constant A
  float B;      // Calibration constant B
};
