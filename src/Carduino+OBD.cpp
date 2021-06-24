#include <Arduino.h>

#include <FreematicsMEMS.h>
#include "Carduino+OBD.h"

COBD obd;
bool connected = false;
unsigned long count = 0;

/**
 * Create an instance of the acceloremeter manager
 */
Carduino_OBD::Carduino_OBD(FreematicsESP32 *sysArg) {
    Serial.println("Initializing OBD unit...");

    // initializations
    while (!sys.begin());
    obd.begin(sys.link);

    Serial.println("OBD unit initialized");
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
    char[OBD_RECV_BUF_SIZE + 1] vinBuffer;
    obd.getVin(vinBuffer, OBD_RECV_BUF_SIZE);
    char[OBD_RECV_BUF_SIZE] = 0;
    Serial.print(vinBuffer)
    Serial.println();
    if (obd.errors > 2) {
        Serial.println("OBD disconnected");
        connected = false;
        obd.reset();
    }
}
