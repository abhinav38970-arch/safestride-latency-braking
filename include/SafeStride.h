// This file defines the core SafeStride math used to compare baseline braking and latency-aware braking.
// The formulas here are the source of truth for threshold expansion and trial margin reporting.
#ifndef SAFESTRIDE_H
#define SAFESTRIDE_H

#include <Arduino.h>

class SafeStride {
private:
    float baseStoppingDistanceMm;

public:
    explicit SafeStride(float baseStoppingDistanceMm);

    float getBaselineThresholdMm() const;
    float calculateBlindDistanceMm(float velocityMmPerSec, uint16_t latencyMs) const;
    float getSafeStrideThresholdMm(float velocityMmPerSec, uint16_t latencyMs) const;
    float estimateRemainingMarginMm(float measuredDistanceMm, float velocityMmPerSec, uint16_t latencyMs) const;
};

#endif
