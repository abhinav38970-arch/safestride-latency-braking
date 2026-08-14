#include <Arduino.h>
#include "config.h"
#include "SafeStride.h"

// Instantiate algorithm with 2.0 inch mechanical baseline
SafeStride safeStride(BASE_STOPPING_DISTANCE_IN);

// Loop timing trackers for measuring jitter (tau)
unsigned long lastLoopTimeMicros = 0;
float currentLoopLatencySec = 0.0;

// Reads ultrasonic distance sensor in inches
float readSensorDistanceInches() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, SENSOR_TIMEOUT_US);
    if (duration == 0) return 999.0; // Out of range / no echo

    // Convert speed of sound (microseconds to inches)
    return (duration * 0.0133) / 2.0; 
}

// Calculates vehicle speed in inches/sec from current motor PWM
float getVehicleVelocity(int currentPWM) {
    return (float)currentPWM * PWM_TO_VELOCITY_SCALE;
}

void setup() {
    Serial.begin(115200);

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(MOTOR_PWM_PIN, OUTPUT);
    pinMode(BRAKE_PIN, OUTPUT);

    digitalWrite(BRAKE_PIN, LOW);
    lastLoopTimeMicros = micros();

    Serial.println("SafeStride AI Firmware Initialized.");
}

void loop() {
    // 1. Calculate loop execution latency (tau) in seconds
    unsigned long currentMicros = micros();
    unsigned long deltaMicros = currentMicros - lastLoopTimeMicros;
    lastLoopTimeMicros = currentMicros;
    
    currentLoopLatencySec = (float)deltaMicros / 1000000.0;

    // 2. Read vehicle state (PWM speed & sensor distance)
    int currentPWM = 250; // Fast tier testing velocity
    float velocity = getVehicleVelocity(currentPWM);
    float obstacleDistance = readSensorDistanceInches();

    // 3. Compute expanded threshold (d_lag = v * tau)
    float activeThreshold = safeStride.getExpandedThreshold(velocity, currentLoopLatencySec);

    // 4. Trigger emergency braking if obstacle enters dynamic buffer
    if (safeStride.checkCollisionRisk(obstacleDistance, velocity, currentLoopLatencySec)) {
        digitalWrite(BRAKE_PIN, HIGH);   // Engages physical brakes
        analogWrite(MOTOR_PWM_PIN, 0);   // Cuts motor power
        
        Serial.print("BRAKE TRIGGERED! Dist: ");
        Serial.print(obstacleDistance);
        Serial.print(" in | Threshold: ");
        Serial.print(activeThreshold);
        Serial.print(" in | Tau: ");
        Serial.println(deltaMicros / 1000.0);
    } else {
        digitalWrite(BRAKE_PIN, LOW);
        analogWrite(MOTOR_PWM_PIN, currentPWM);
    }

    // Small delay simulating background multi-sensor processing load
    delay(10); 
}
