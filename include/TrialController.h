// This file defines the experiment state machine that runs baseline or SafeStride trials from parsed commands.
// It owns braking decisions, motor actions, and trial telemetry reporting.
#ifndef TRIAL_CONTROLLER_H
#define TRIAL_CONTROLLER_H

#include <Arduino.h>

#include "MotorControl.h"
#include "SafeStride.h"
#include "SerialCommand.h"
#include "ToFSensor.h"

class TrialController {
private:
    SafeStride safeStride;
    MotorControl motor;
    ToFSensor sensor;

    TrialCommand activeCommand;
    bool trialActive;
    unsigned long trialStartMs;

    float currentThresholdMm(float velocityMmPerSec) const;
    const char* systemTypeName() const;
    void printTrialArmed() const;
    void printTrialResult(
        const char* eventLabel,
        uint16_t distanceMm,
        float thresholdMm,
        float blindDistanceMm,
        float marginMm,
        bool collision);

public:
    TrialController();

    void begin();
    void startTrial(const TrialCommand& command);
    void update(float measuredLoopLatencySec);
};

#endif
