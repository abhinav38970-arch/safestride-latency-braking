#include "SafeStride.h"

SafeStride::SafeStride(float baseStoppingDist) {
    baseThreshold = baseStoppingDist;
}

// d_lag = v * tau
float SafeStride::calculateBlindDistance(float velocity, float loopLatencySec) {
    return velocity * loopLatencySec;
}

// Dynamic Expansion: Total Threshold = Base Braking Distance + Blind Travel Distance
float SafeStride::getExpandedThreshold(float velocity, float loopLatencySec) {
    float blindDistance = calculateBlindDistance(velocity, loopLatencySec);
    return baseThreshold + blindDistance;
}

// Emergency brake trigger decision
bool SafeStride::checkCollisionRisk(float currentDistance, float velocity, float loopLatencySec) {
    float safeDistance = getExpandedThreshold(velocity, loopLatencySec);
    return (currentDistance <= safeDistance);
}
