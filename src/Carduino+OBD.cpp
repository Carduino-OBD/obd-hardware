#include <Arduino.h>

#include "Carduino+OBD.h"
#include <FreematicsPlus.h>
#define RUNNING_VOLTAGE 14

COBD obd;
unsigned long count = 0;
bool inLowPowerMode;

/**
 * Create an instance of the acceloremeter manager
 */
Carduino_OBD::Carduino_OBD(FreematicsESP32 *sysArg, Carduino_Accelerometer *accelerometerUnit) {
    Serial.println("Initializing OBD unit...");

    Serial.print("TYPE:");
    Serial.println(sysArg->devType);
    obd.begin(sysArg->link);

    Serial.print("Connecting to OBD...");
    if (obd.init()) {
        Serial.println("OK");
        this->connected = true;
    } else {
        Serial.println();
        this->connected = false;
    }

    inLowPowerMode = false;

    this->accelerometerUnit = accelerometerUnit;

    Serial.println("OBD initialized");
}

void Carduino_OBD::runLoop(void) {
    const float motionThreshold = 0.2;
    if (inLowPowerMode && this->accelerometerUnit->getMotion() < motionThreshold &&
        obd.getVoltage() < RUNNING_VOLTAGE) {
        pinMode(PIN_LED, HIGH);
        // delay(5000);
        return;
    } else if (!this->connected && this->accelerometerUnit->getMotion() < motionThreshold &&
               obd.getVoltage() < RUNNING_VOLTAGE && !inLowPowerMode) {
        Serial.println("OBD not connected and motion below threshold, low power");
        inLowPowerMode = true;
        obd.enterLowPowerMode();
        return;
    } else if (!this->connected) {
        inLowPowerMode = false;
        obd.leaveLowPowerMode();
        Serial.print("Connecting to OBD...");
        if (obd.init()) {
            Serial.println("OK");
            this->connected = true;
        } else {
            Serial.println();
            return;
        }
    }

    int value;
    // Serial.print('[');
    // Serial.print(millis());
    // Serial.print("] #");
    // Serial.print(count++);
    obd.readPID(PID_RPM, value);
    if (obd.readPID(PID_RPM, value)) {
        // Serial.print(" RPM:");
        // Serial.print(value);
    }
    obd.readPID(PID_SPEED, value);
    if (obd.readPID(PID_SPEED, value)) {
        this->vehicleSpeed = value;
        // Serial.print(" SPEED:");
        // Serial.print(value);
    }
    obd.getVoltage();
    // Serial.print(" BATTERY:");
    // Serial.print(obd.getVoltage());
    // Serial.print('V');

    // Serial.print(" CPU TEMP:");
    // Serial.print(readChipTemperature());

    // Serial.print(" VIN: ");

    obd.getVIN(vinBuffer, OBD_RECV_BUF_SIZE);
    vinBuffer[OBD_RECV_BUF_SIZE] = 0;
    // Serial.print(vinBuffer);
    // Serial.println();
    if (obd.errors > 2) {
        Serial.println("OBD disconnected");
        this->connected = false;
        obd.reset();
    }
}

uint8_t Carduino_OBD::getVehicleSpeed() { return this->vehicleSpeed; }

bool Carduino_OBD::isConnected() { return this->connected; }
