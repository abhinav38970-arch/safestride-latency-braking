// This file holds shared hardware pins and experiment constants for the SafeStride braking firmware.
// Keeping these values in one place makes it easy to tune the test setup without changing control logic.
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

constexpr uint32_t SERIAL_BAUD = 115200;

// ESP32 + L298N motor driver wiring.
constexpr uint8_t MOTOR_ENA_PIN = 25;
constexpr uint8_t MOTOR_IN1_PIN = 26;
constexpr uint8_t MOTOR_IN2_PIN = 27;

// ESP32 I2C wiring for VL53L1X.
constexpr uint8_t I2C_SDA_PIN = 21;
constexpr uint8_t I2C_SCL_PIN = 22;

// Trial physics parameters.
constexpr float BASE_STOPPING_DISTANCE_MM = 51.0f;      // ~2 inches mechanical stopping distance.
constexpr float PWM_TO_MM_PER_SEC = 2.03f;              // Empirical conversion from PWM to speed.
constexpr float COLLISION_MARGIN_MM = 0.0f;             // Negative margin means collision.

// Sensor and trial timing.
constexpr uint16_t SENSOR_POLL_INTERVAL_MS = 20;
constexpr uint16_t SENSOR_STALE_MS = 300;
constexpr uint16_t ACTIVE_SHORT_BRAKE_MS = 120;
constexpr uint32_t TRIAL_TIMEOUT_MS = 8000;

// Serial command parser limits.
constexpr size_t MAX_COMMAND_LENGTH = 80;

#endif
