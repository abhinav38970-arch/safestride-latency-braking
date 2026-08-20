// This file implements the L298N output signals for normal drive and emergency active braking.
// Active short braking is done by driving both H-bridge sides high for a brief braking pulse.
#include "MotorControl.h"
#include "config.h"

void MotorControl::begin() {
    pinMode(MOTOR_ENA_PIN, OUTPUT);
    pinMode(MOTOR_IN1_PIN, OUTPUT);
    pinMode(MOTOR_IN2_PIN, OUTPUT);
    stopCoast();
}

void MotorControl::driveForward(uint8_t pwm) {
    digitalWrite(MOTOR_IN1_PIN, HIGH);
    digitalWrite(MOTOR_IN2_PIN, LOW);
    analogWrite(MOTOR_ENA_PIN, pwm);
}

void MotorControl::stopCoast() {
    analogWrite(MOTOR_ENA_PIN, 0);
    digitalWrite(MOTOR_IN1_PIN, LOW);
    digitalWrite(MOTOR_IN2_PIN, LOW);
}

void MotorControl::activeShortBrake(uint16_t durationMs) {
    analogWrite(MOTOR_ENA_PIN, 255);
    digitalWrite(MOTOR_IN1_PIN, HIGH);
    digitalWrite(MOTOR_IN2_PIN, HIGH);
    delay(durationMs);
    stopCoast();
}
