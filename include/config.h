#ifndef CONFIG_H
#define CONFIG_H

// --- Hardware Pin Definitions (ESP32) ---
// VL53L1X time-of-flight distance sensor on I2C
#define TOF_SDA_PIN   21   // ESP32 default I2C SDA
#define TOF_SCL_PIN   22   // ESP32 default I2C SCL
// Drive hardware
#define MOTOR_PWM_PIN 25   // Motor speed control (L298N-class driver)
#define BRAKE_PIN     26   // Physical brake relay / signal

// --- SafeStride Parameters (SI units) ---
// Base trigger distance: the fixed-distance threshold used by the Baseline
// controller, in meters. SafeStride expands this by the latency-distance term.
const float D_BASE_M = 0.15;

// Nominal forward velocities per commanded PWM setting (m/s).
// Firmware setpoints used by the latency-distance term; these were not
// independently calibrated against measured physical speed.
const float V0_PWM130_MS = 0.65;
const float V0_PWM190_MS = 0.90;
const float V0_PWM250_MS = 1.25;

// --- Tested Conditions (600-trial experiment) ---
// Commanded PWM speed settings
const int PWM_SETTINGS[3] = {130, 190, 250};
// Injected software loop delays (ms)
const int INJECTED_LATENCY_MS[5] = {10, 50, 100, 150, 200};

#endif // CONFIG_H
