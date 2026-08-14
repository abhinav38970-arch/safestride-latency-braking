#ifndef SAFESTRIDE_H
#define SAFESTRIDE_H

#include <Arduino.h>

class SafeStride {
private:
    float baseThreshold; // Base mechanical stopping distance (inches)

public:
    SafeStride(float baseStoppingDist);

    // Calculates unbraked distance covered during processor lag: d_lag = v * tau
    float calculateBlindDistance(float velocity, float loopLatencySec);

    // Dynamically expands the braking distance threshold
    float getExpandedThreshold(float velocity, float loopLatencySec);

    // Evaluates current obstacle distance against expanded safety buffer
    bool checkCollisionRisk(float currentDistance, float velocity, float loopLatencySec);
};

#endif // SAFESTRIDE_H
