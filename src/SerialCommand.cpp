// This file implements robust serial parsing so malformed commands do not crash or arm a bad trial.
// Only valid baseline or SafeStride commands are accepted and reported back over Serial.
#include "SerialCommand.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void SerialCommandParser::trimWhitespace(char* text) {
    if (text == nullptr) {
        return;
    }

    size_t start = 0;
    size_t length = strlen(text);
    while (start < length && isspace(static_cast<unsigned char>(text[start]))) {
        start++;
    }

    size_t end = length;
    while (end > start && isspace(static_cast<unsigned char>(text[end - 1]))) {
        end--;
    }

    const size_t newLength = end - start;
    if (start > 0 && newLength > 0) {
        memmove(text, text + start, newLength);
    }
    text[newLength] = '\0';
}

SerialCommandParser::SerialCommandParser() : buffer{0}, writeIndex(0) {}

bool SerialCommandParser::parseLine(const char* line, TrialCommand& command) const {
    if (line == nullptr) {
        return false;
    }

    const size_t len = strlen(line);
    if (len < 5 || line[0] != '<' || line[len - 1] != '>') {
        return false;
    }

    char working[MAX_COMMAND_LENGTH + 1] = {0};
    if (len - 2 >= sizeof(working)) {
        return false;
    }

    memcpy(working, line + 1, len - 2);
    working[len - 2] = '\0';

    char* tokenSystem = strtok(working, ",");
    char* tokenLatency = strtok(nullptr, ",");
    char* tokenPwm = strtok(nullptr, ",");
    char* tokenExtra = strtok(nullptr, ",");

    if (tokenSystem == nullptr || tokenLatency == nullptr || tokenPwm == nullptr || tokenExtra != nullptr) {
        return false;
    }

    trimWhitespace(tokenSystem);
    trimWhitespace(tokenLatency);
    trimWhitespace(tokenPwm);

    for (char* p = tokenSystem; *p != '\0'; ++p) {
        *p = static_cast<char>(toupper(static_cast<unsigned char>(*p)));
    }

    TrialCommand::SystemType systemType;
    if (strcmp(tokenSystem, "BASELINE") == 0 || strcmp(tokenSystem, "0") == 0) {
        systemType = TrialCommand::SystemType::Baseline;
    } else if (strcmp(tokenSystem, "SAFESTRIDE") == 0 || strcmp(tokenSystem, "1") == 0) {
        systemType = TrialCommand::SystemType::SafeStride;
    } else {
        return false;
    }

    char* latencyEnd = nullptr;
    long latencyLong = strtol(tokenLatency, &latencyEnd, 10);
    if (latencyEnd == tokenLatency || *latencyEnd != '\0' || latencyLong < 0 || latencyLong > 60000) {
        return false;
    }

    char* pwmEnd = nullptr;
    long pwmLong = strtol(tokenPwm, &pwmEnd, 10);
    if (pwmEnd == tokenPwm || *pwmEnd != '\0' || pwmLong < 0 || pwmLong > 255) {
        return false;
    }

    command.systemType = systemType;
    command.latencyMs = static_cast<uint16_t>(latencyLong);
    command.pwm = static_cast<uint8_t>(pwmLong);
    return true;
}

bool SerialCommandParser::poll(TrialCommand& command) {
    while (Serial.available() > 0) {
        const char c = static_cast<char>(Serial.read());

        if (c == '\n' || c == '\r') {
            if (writeIndex == 0) {
                continue;
            }

            buffer[writeIndex] = '\0';
            writeIndex = 0;

            if (parseLine(buffer, command)) {
                return true;
            }

            Serial.println("ERR: Expected <SystemType, Latency_ms, PWM>");
            return false;
        }

        if (writeIndex < sizeof(buffer) - 1) {
            buffer[writeIndex++] = c;
        } else {
            writeIndex = 0;
            Serial.println("ERR: Command too long");
            return false;
        }
    }

    return false;
}
