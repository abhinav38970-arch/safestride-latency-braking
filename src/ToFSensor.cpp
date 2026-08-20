// This file implements VL53L1X initialization and non-blocking sample polling for each firmware loop.
// It keeps a cached measurement so braking logic can run quickly without waiting on I2C reads.
#include "ToFSensor.h"

#include <Wire.h>

#include "config.h"

ToFSensor::ToFSensor()
    : initialized(false), latestRangeMm(0), lastUpdateMs(0) {}

bool ToFSensor::begin() {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

    sensor.setTimeout(50);
    if (!sensor.init()) {
        initialized = false;
        return false;
    }

    sensor.setDistanceMode(VL53L1X::Long);
    sensor.setMeasurementTimingBudget(50000);
    sensor.startContinuous(SENSOR_POLL_INTERVAL_MS);

    initialized = true;
    return true;
}

void ToFSensor::update() {
    if (!initialized) {
        return;
    }

    if (sensor.dataReady()) {
        latestRangeMm = sensor.read();
        sensor.clearInterrupt();
        lastUpdateMs = millis();
    }
}

bool ToFSensor::isInitialized() const {
    return initialized;
}

bool ToFSensor::hasRecentMeasurement(uint16_t maxAgeMs) const {
    return initialized && lastUpdateMs > 0 && (millis() - lastUpdateMs) <= maxAgeMs;
}

uint16_t ToFSensor::getLatestRangeMm() const {
    return latestRangeMm;
}
