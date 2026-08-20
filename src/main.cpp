// This file is the firmware entry point that wires serial commands to the SafeStride trial controller.
// It keeps setup/loop thin so experiment behavior stays organized in dedicated modules.
#include <Arduino.h>

#include "SerialCommand.h"
#include "TrialController.h"
#include "config.h"

namespace {
SerialCommandParser commandParser;
TrialController trialController;
unsigned long lastLoopMicros = 0;
}

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(150);

    Serial.println("SafeStride latency braking firmware started");
    trialController.begin();
    lastLoopMicros = micros();
}

void loop() {
    const unsigned long now = micros();
    const float loopLatencySec = static_cast<float>(now - lastLoopMicros) / 1000000.0f;
    lastLoopMicros = now;

    TrialCommand command;
    if (commandParser.poll(command)) {
        trialController.startTrial(command);
    }

    trialController.update(loopLatencySec);
}
