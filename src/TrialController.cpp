// This file implements the runtime trial flow including threshold checks, active braking, and telemetry output.
// It is where baseline behavior and SafeStride behavior are applied side by side for comparison.
#include "TrialController.h"

#include "config.h"

TrialController::TrialController()
    : safeStride(BASE_STOPPING_DISTANCE_MM),
      activeCommand{TrialCommand::SystemType::Baseline, 0, 0},
      trialActive(false),
      trialStartMs(0) {}

void TrialController::begin() {
    motor.begin();
    const bool sensorReady = sensor.begin();

    Serial.print("SENSOR_STATUS,");
    Serial.println(sensorReady ? "READY" : "NOT_READY");
}

void TrialController::startTrial(const TrialCommand& command) {
    activeCommand = command;
    trialActive = true;
    trialStartMs = millis();

    motor.driveForward(activeCommand.pwm);
    printTrialArmed();
}

float TrialController::currentThresholdMm(float velocityMmPerSec) const {
    if (activeCommand.systemType == TrialCommand::SystemType::Baseline) {
        return safeStride.getBaselineThresholdMm();
    }

    return safeStride.getSafeStrideThresholdMm(velocityMmPerSec, activeCommand.latencyMs);
}

const char* TrialController::systemTypeName() const {
    return activeCommand.systemType == TrialCommand::SystemType::Baseline ? "BASELINE" : "SAFESTRIDE";
}

void TrialController::printTrialArmed() const {
    Serial.print("TRIAL_ARMED,System=");
    Serial.print(systemTypeName());
    Serial.print(",Latency_ms=");
    Serial.print(activeCommand.latencyMs);
    Serial.print(",PWM=");
    Serial.println(activeCommand.pwm);
}

void TrialController::printTrialResult(
    const char* eventLabel,
    uint16_t distanceMm,
    float thresholdMm,
    float blindDistanceMm,
    float marginMm,
    bool collision) {
    Serial.print("TRIAL_RESULT,Event=");
    Serial.print(eventLabel);
    Serial.print(",System=");
    Serial.print(systemTypeName());
    Serial.print(",Latency_ms=");
    Serial.print(activeCommand.latencyMs);
    Serial.print(",PWM=");
    Serial.print(activeCommand.pwm);
    Serial.print(",Distance_mm=");
    Serial.print(distanceMm);
    Serial.print(",Threshold_mm=");
    Serial.print(thresholdMm, 2);
    Serial.print(",BlindDistance_mm=");
    Serial.print(blindDistanceMm, 2);
    Serial.print(",Margin_mm=");
    Serial.print(marginMm, 2);
    Serial.print(",Collision=");
    Serial.println(collision ? 1 : 0);
}

void TrialController::update(float measuredLoopLatencySec) {
    (void)measuredLoopLatencySec;

    sensor.update();
    if (!trialActive) {
        return;
    }

    if (millis() - trialStartMs > TRIAL_TIMEOUT_MS) {
        motor.stopCoast();
        printTrialResult("TIMEOUT", sensor.getLatestRangeMm(), 0.0f, 0.0f, 0.0f, false);
        trialActive = false;
        return;
    }

    if (!sensor.hasRecentMeasurement(SENSOR_STALE_MS)) {
        return;
    }

    const uint16_t distanceMm = sensor.getLatestRangeMm();
    const float velocityMmPerSec = static_cast<float>(activeCommand.pwm) * PWM_TO_MM_PER_SEC;
    const float blindDistanceMm = safeStride.calculateBlindDistanceMm(velocityMmPerSec, activeCommand.latencyMs);
    const float thresholdMm = currentThresholdMm(velocityMmPerSec);

    if (distanceMm > thresholdMm) {
        motor.driveForward(activeCommand.pwm);
        return;
    }

    if (activeCommand.latencyMs > 0) {
        delay(activeCommand.latencyMs);
    }

    if (activeCommand.systemType == TrialCommand::SystemType::SafeStride) {
        motor.activeShortBrake(ACTIVE_SHORT_BRAKE_MS);
    } else {
        motor.stopCoast();
    }

    const float marginMm = safeStride.estimateRemainingMarginMm(distanceMm, velocityMmPerSec, activeCommand.latencyMs);
    const bool collision = marginMm < COLLISION_MARGIN_MM;
    printTrialResult("BRAKE", distanceMm, thresholdMm, blindDistanceMm, marginMm, collision);

    trialActive = false;
}
