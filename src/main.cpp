#include <Arduino.h>
#include <Wire.h>
#include <VL53L1X.h>   // Pololu VL53L1X library
#include "config.h"
#include "SafeStride.h"

// SafeStride controller with the experimental base trigger distance (m)
SafeStride safeStride(D_BASE_M);

VL53L1X tofSensor;

// --- Test condition under evaluation (set per experimental run) ---
// The 600-trial experiment crossed 3 PWM settings x 5 injected delays;
// the values below select one cell of that grid per firmware flash.
const int   CURRENT_PWM = 250;
const float CURRENT_V0_MS = V0_PWM250_MS;   // nominal velocity for this PWM
const int   CURRENT_INJECTED_LATENCY_MS = 200;

unsigned long lastLoopTimeMicros = 0;

// Reads the VL53L1X time-of-flight sensor; returns distance in meters
float readTofDistanceMeters() {
    uint16_t distMm = tofSensor.readRangeContinuousMillimeters();
    if (tofSensor.timeoutOccurred()) return 999.0; // out of range / no target
    return (float)distMm / 1000.0;
}

// Maps the commanded PWM setting to its nominal firmware velocity (m/s)
float nominalVelocity(int pwm) {
    if (pwm == 130) return V0_PWM130_MS;
    if (pwm == 190) return V0_PWM190_MS;
    return V0_PWM250_MS;
}

void setup() {
    Serial.begin(115200);

    Wire.begin(TOF_SDA_PIN, TOF_SCL_PIN);
    tofSensor.setTimeout(500);
    if (!tofSensor.init()) {
        Serial.println("VL53L1X not detected. Check I2C wiring.");
        while (1) ;
    }
    tofSensor.setDistanceMode(VL53L1X::Long);
    tofSensor.setMeasurementTimingBudget(20000);
    tofSensor.startContinuous(50);

    pinMode(MOTOR_PWM_PIN, OUTPUT);
    pinMode(BRAKE_PIN, OUTPUT);
    digitalWrite(BRAKE_PIN, LOW);

    lastLoopTimeMicros = micros();
    Serial.println("SafeStride firmware initialized (VL53L1X, SI units).");
}

void loop() {
    // 1. Measure loop latency (tau) in seconds, verified with micros() timing
    unsigned long currentMicros = micros();
    float loopLatencySec = (float)(currentMicros - lastLoopTimeMicros) / 1000000.0;
    lastLoopTimeMicros = currentMicros;

    // 2. Read vehicle state: nominal velocity for this PWM + ToF distance
    float velocity = nominalVelocity(CURRENT_PWM);
    float obstacleDistance = readTofDistanceMeters();

    // 3. Compute expanded trigger: D_safe = D_base + v0 * tau
    float activeThreshold = safeStride.getExpandedThreshold(velocity, loopLatencySec);

    // 4. Trigger emergency braking if the obstacle enters the dynamic buffer
    if (safeStride.checkCollisionRisk(obstacleDistance, velocity, loopLatencySec)) {
        digitalWrite(BRAKE_PIN, HIGH);   // engage physical brake
        analogWrite(MOTOR_PWM_PIN, 0);   // cut motor power

        Serial.print("BRAKE TRIGGERED! Dist: ");
        Serial.print(obstacleDistance, 3);
        Serial.print(" m | Threshold: ");
        Serial.print(activeThreshold, 3);
        Serial.print(" m | Tau: ");
        Serial.print(loopLatencySec * 1000.0, 1);
        Serial.println(" ms");
    } else {
        digitalWrite(BRAKE_PIN, LOW);
        analogWrite(MOTOR_PWM_PIN, CURRENT_PWM);
    }

    // Injected software latency for this test condition (the experiment's
    // delay variable: 10, 50, 100, 150, or 200 ms)
    delay(CURRENT_INJECTED_LATENCY_MS);
}
