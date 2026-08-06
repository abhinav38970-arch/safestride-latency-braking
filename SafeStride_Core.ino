/*
 * SafeStride AI: Latency-Aware Emergency Braking
 * Main Controller Firmware for ESP32/Arduino
 * * Logic: Dynamically adjusts braking threshold based on real-time loop latency 
 * to eliminate the "spatial blind spot" on budget hardware.
 */

#include "SensorHandler.h"

// --- Hardware Pins ---
const int ENA = 9;  // Motor Speed (PWM)
const int IN1 = 8;  // Motor Direction 1
const int IN2 = 7;  // Motor Direction 2
const int TRIG = 3; // Sensor Trigger
const int ECHO = 4; // Sensor Echo

// --- Physics & AI Constants ---
const float deceleration = 1.5;   // Braking capability (m/s^2)
const float safetyBuffer = 0.05;  // 5cm hard limit buffer (m)
float currentVelocity = 0.6;      // Approx max speed of robot (m/s)

// --- Timing Variables ---
unsigned long lastLoopMicros = 0;
float currentLatency = 0;

// Initialize Sensor
SensorHandler frontSensor(TRIG, ECHO);

void setup() {
  Serial.begin(115200);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  
  // Start moving forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 250); // Start at High Speed (250 PWM)
  
  lastLoopMicros = micros();
}

void loop() {
  // 1. MEASURE LATENCY (tau)
  unsigned long currentMicros = micros();
  currentLatency = (currentMicros - lastLoopMicros) / 1000000.0; // Convert to seconds
  lastLoopMicros = currentMicros;

  // 2. READ SMOOTHED SENSOR DATA
  float distanceToObstacle = frontSensor.getSmoothedDistance();

  // 3. THE AI PHYSICS CORE: Calculate Dynamic Threshold
  // d_lag = v * latency
  // d_brake = (v^2) / (2 * a)
  float blindSpotDistance = currentVelocity * currentLatency;
  float mechanicalBrakingDistance = pow(currentVelocity, 2) / (2 * deceleration);
  
  float dynamicThreshold = blindSpotDistance + mechanicalBrakingDistance + safetyBuffer;

  // 4. PREEMPTIVE BRAKING DECISION
  if (distanceToObstacle <= dynamicThreshold) {
    applyEmergencyBrake();
  }

  // (Optional) Simulate artificial lag for testing based on your trials
  // delay(50); // Simulating 50ms latency
}

void applyEmergencyBrake() {
  // Hard stop motors
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
  
  Serial.print("CRITICAL: Preemptive brake engaged! ");
  Serial.print("Latency spike of ");
  Serial.print(currentLatency * 1000);
  Serial.println("ms detected.");
  
  // Lock the system to prevent further movement
  while(true) {
    delay(1000);
  }
}
