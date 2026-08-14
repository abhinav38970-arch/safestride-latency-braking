#ifndef CONFIG_H
#define CONFIG_H

// --- Hardware Pin Definitions (ESP32) ---
#define TRIG_PIN       5    // Ultrasonic Trigger
#define ECHO_PIN       18   // Ultrasonic Echo
#define MOTOR_PWM_PIN  25   // Motor Speed Control
#define BRAKE_PIN      26   // Physical Brake Relay / Signal

// --- Baseline Physical Parameters ---
// Static braking distance required at rest / minimal speed (inches)
const float BASE_STOPPING_DISTANCE_IN = 2.0; 

// Conversion factor: Maps PWM motor output (0-255) to physical velocity (inches/sec)
// Example: PWM 250 ~ 20.0 inches/sec on testing rig
const float PWM_TO_VELOCITY_SCALE = 0.08; 

// Sensor timeout limits (microseconds)
const unsigned long SENSOR_TIMEOUT_US = 30000;

#endif // CONFIG_H
