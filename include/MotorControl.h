// This file defines a small driver for L298N motor commands used by the braking experiment.
// It exposes forward drive, coast stop, and active short-brake behavior.
#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

class MotorControl {
public:
    void begin();
    void driveForward(uint8_t pwm);
    void stopCoast();
    void activeShortBrake(uint16_t durationMs);
};

#endif
