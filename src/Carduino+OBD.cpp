#include <Arduino.h>

#include <FreematicsPlus.h>
#include "Carduino+OBD.h"
#define OBD_RECV_BUF_SIZE 80

COBD obd;
bool connected = false;
unsigned long count = 0;

/**
 * Create an instance of the acceloremeter manager
 */
Carduino_OBD::Carduino_OBD(FreematicsESP32 *sysArg) {
    Serial.println("Initializing OBD unit...");

    Serial.print("TYPE:");
    Serial.println(sysArg->devType);
    obd.begin(sysArg->link);
    Serial.println(" OBD initialized");


}


void Carduino_OBD::runLoop(void) {
    if (!connected) {
        Serial.print("Connecting to OBD...");
        if (obd.init()) {
            Serial.println("OK");
            connected = true;
        } else {
            Serial.println();
        }
        return;
    }

    int value;
    Serial.print('[');
    Serial.print(millis());
    Serial.print("] #");
    Serial.print(count++);
    if (obd.readPID(PID_RPM, value)) {
        Serial.print(" RPM:");
        Serial.print(value);
    }
    if (obd.readPID(PID_SPEED, value)) {
        Serial.print(" SPEED:");
        Serial.print(value);
    }

    Serial.print(" BATTERY:");
    Serial.print(obd.getVoltage());
    Serial.print('V');

    Serial.print(" CPU TEMP:");
    Serial.print(readChipTemperature());

    Serial.print(" VIN: ");
    char vinBuffer[OBD_RECV_BUF_SIZE + 1];
    obd.getVIN(vinBuffer, OBD_RECV_BUF_SIZE);
    vinBuffer[OBD_RECV_BUF_SIZE] = 0;
    Serial.print(vinBuffer);
    Serial.println();
    if (obd.errors > 2) {
        Serial.println("OBD disconnected");
        connected = false;
        obd.reset();
    }
}
