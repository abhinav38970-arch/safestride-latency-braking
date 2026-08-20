// This file defines parsing for commands in the form <SystemType, Latency_ms, PWM> sent over Serial.
// It validates input and converts it into a typed trial configuration for the controller.
#ifndef SERIAL_COMMAND_H
#define SERIAL_COMMAND_H

#include <Arduino.h>
#include "config.h"

struct TrialCommand {
    enum class SystemType {
        Baseline,
        SafeStride
    };

    SystemType systemType;
    uint16_t latencyMs;
    uint8_t pwm;
};

class SerialCommandParser {
private:
    char buffer[MAX_COMMAND_LENGTH + 1];
    size_t writeIndex;

    bool parseLine(const char* line, TrialCommand& command) const;
    static void trimWhitespace(char* text);

public:
    SerialCommandParser();
    bool poll(TrialCommand& command);
};

#endif
