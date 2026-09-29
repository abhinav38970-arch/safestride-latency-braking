# SafeStride: Latency-Aware Braking for a Low-Cost Mobile Robot

An exploratory 600-trial study of a deterministic latency-compensation rule for
obstacle braking. A conventional fixed-distance trigger brakes when the measured
obstacle distance falls below a constant threshold. SafeStride instead shifts
that trigger earlier by a nominal latency-distance term:

```
D_safe = D_base + v0 * tau
```

where `D_base` is the base trigger distance, `v0` is the nominal forward
velocity for the commanded PWM setting, and `tau` is the measured control-loop
latency. One multiplication and one addition per control loop.

## Hardware

- OSOYOO V2.1 mechanical robot-car platform (chassis, DC gear motors, wheels)
- ESP32 controller
- L298N-class motor driver
- VL53L1X time-of-flight distance sensor (I2C)

## Firmware parameters (as reported in the paper)

- `D_base = 0.15 m`
- Nominal velocities: PWM 130 → 0.65 m/s, PWM 190 → 0.90 m/s, PWM 250 → 1.25 m/s
  (firmware setpoints; not independently calibrated against measured speed)
- Loop latency `tau` measured with `micros()` timing each control loop
- Tested grid: 3 PWM settings × 5 injected software delays
  (10, 50, 100, 150, 200 ms) × 20 trials per cell × 2 controllers
  (Baseline fixed trigger, then SafeStride) = 600 trials

Requires the [Pololu VL53L1X Arduino library](https://github.com/pololu/vl53l1x-arduino).

## Data

- `Final Autonomous Braking Complete Data Matrix ... .csv` — pre-algorithm
  (Baseline) trial stopping margins in inches; `CRASH` marks a collision.
- `post_algo_trials.csv` — post-algorithm (SafeStride) trial-level data:
  trial ID, condition, PWM, injected latency (ms), trial-in-condition, and
  stopping margin in inches. A negative margin marks a collision
  (the robot's stopping point was past the wall plane).

## Results (exploratory)

Baseline: 66/300 collisions (22.0%). SafeStride: 21/300 (7.0%).
Observed risk ratio 0.318 (95% CI 0.200–0.506), risk difference −15.0
percentage points (95% CI −20.5 to −9.4). The two controllers were run as
sequential phases, so the comparison is an exploratory pre/post association,
not a causal estimate. See the paper for the full analysis, limitations,
and the failure-boundary pattern.

## Note on this repository's history

An earlier prototype iteration of this firmware used an HC-SR04 ultrasonic
sensor and inch units. The code here reflects the final experimental
configuration described in the paper (VL53L1X, SI units, `D_base = 0.15 m`).
