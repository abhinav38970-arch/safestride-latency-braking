// This file implements the SafeStride equations that account for blind travel during processing latency.
// It is intentionally small so trial-control code can stay readable and easy to verify.
#include "SafeStride.h"

SafeStride::SafeStride(float baseStoppingDistanceMm)
    : baseStoppingDistanceMm(baseStoppingDistanceMm) {}

float SafeStride::getBaselineThresholdMm() const {
    return baseStoppingDistanceMm;
}

float SafeStride::calculateBlindDistanceMm(float velocityMmPerSec, uint16_t latencyMs) const {
    return velocityMmPerSec * (static_cast<float>(latencyMs) / 1000.0f);
}

float SafeStride::getSafeStrideThresholdMm(float velocityMmPerSec, uint16_t latencyMs) const {
    return baseStoppingDistanceMm + calculateBlindDistanceMm(velocityMmPerSec, latencyMs);
}

float SafeStride::estimateRemainingMarginMm(float measuredDistanceMm, float velocityMmPerSec, uint16_t latencyMs) const {
    return measuredDistanceMm - getSafeStrideThresholdMm(velocityMmPerSec, latencyMs);
}
