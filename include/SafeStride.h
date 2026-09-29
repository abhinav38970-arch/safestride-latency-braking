#ifndef SAFESTRIDE_H
#define SAFESTRIDE_H

#include <Arduino.h>

class SafeStride {
private:
    float baseThreshold; // Base trigger distance, D_base (meters)

public:
    SafeStride(float baseStoppingDistM);

    // Calculates unbraked distance covered during loop latency: d_lag = v0 * tau
    float calculateBlindDistance(float velocity, float loopLatencySec);

    // Dynamically expands the braking trigger: D_safe = D_base + v0 * tau
    float getExpandedThreshold(float velocity, float loopLatencySec);

    // Evaluates current obstacle distance against the expanded safety buffer
    bool checkCollisionRisk(float currentDistance, float velocity, float loopLatencySec);
};

#endif // SAFESTRIDE_H
