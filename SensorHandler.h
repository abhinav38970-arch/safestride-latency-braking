/*
 * SafeStride AI - SensorHandler.h
 * Handles raw data from ultrasonic sensors and applies a moving average 
 * filter to eliminate high-frequency noise and false reflections.
 */

#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include <Arduino.h>

class SensorHandler {
  private:
    int trigPin;
    int echoPin;
    
    // Variables for smoothing data
    static const int numReadings = 5;
    float readings[numReadings];
    int readIndex = 0;
    float total = 0;

  public:
    SensorHandler(int trig, int echo) {
      trigPin = trig;
      echoPin = echo;
      
      pinMode(trigPin, OUTPUT);
      pinMode(echoPin, INPUT);

      // Initialize readings array to 0
      for (int i = 0; i < numReadings; i++) {
        readings[i] = 0;
      }
    }

    // Pings the sensor and returns the raw distance in meters
    float getRawDistance() {
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);

      float duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout
      if (duration == 0) return 999.0; // Return safe distance if no echo
      
      // Speed of sound is ~343 m/s. 
      return (duration * 0.0343) / 200.0; // Convert to meters
    }

    // Returns a smoothed distance to prevent false braking
    float getSmoothedDistance() {
      total = total - readings[readIndex];
      readings[readIndex] = getRawDistance();
      total = total + readings[readIndex];
      readIndex = (readIndex + 1) % numReadings;
      
      return total / numReadings;
    }
};

#endif
