// This file defines a VL53L1X wrapper that updates range readings without blocking the control loop.
// The controller reads the latest cached distance while this module handles sensor-ready polling.
#ifndef TOF_SENSOR_H
#define TOF_SENSOR_H

#include <Arduino.h>
#include <VL53L1X.h>

class ToFSensor {
private:
    VL53L1X sensor;
    bool initialized;
    uint16_t latestRangeMm;
    unsigned long lastUpdateMs;

public:
    ToFSensor();

    bool begin();
    void update();

    bool isInitialized() const;
    bool hasRecentMeasurement(uint16_t maxAgeMs) const;
    uint16_t getLatestRangeMm() const;
};

#endif
