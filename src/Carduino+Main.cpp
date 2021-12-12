#include "Carduino+Main.h"
#include <Arduino.h>

FreematicsESP32 sys;

// obd.enterLowPowerMode();
/**
 * Create an instance of Carduino
 */
Carduino_Main::Carduino_Main() {
    Serial.begin(115200);
    sys.begin(1, 1);
    Serial.println("Initializing Carduino...");

    uint64_t chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
    uint16_t chip = (uint16_t)(chipid >> 32);

    snprintf(this->serial, 22, "CARDUINO-%04X%08X", chip, (uint32_t)chipid);

    Serial.printf("Serial number: %s\n", this->serial);

    this->accelerometerAgent = new Carduino_Accelerometer();
    this->deviceAgent = new Carduino_Device(&sys);
    this->gpsAgent = new Carduino_GPS(&sys);
    this->obdAgent = new Carduino_OBD(&sys, this->accelerometerAgent);
    this->bluetoothAgent = new Carduino_Bluetooth(this->gpsAgent, this->serial);
    this->driveAgent = new Carduino_DriveManager(this->gpsAgent, this->accelerometerAgent, this->obdAgent);
    Serial.println("Done initializing");
}

/**
 * Called every iteration of the arduino run loop
 */
void Carduino_Main::runLoop(void) {
    this->accelerometerAgent->runLoop();
    this->gpsAgent->runLoop();
    this->deviceAgent->runLoop();
    this->obdAgent->runLoop();
    this->bluetoothAgent->runLoop();
    this->driveAgent->runLoop();
}
