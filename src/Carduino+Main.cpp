#include "Carduino+Main.h"
#include <Arduino.h>

FreematicsESP32 sys;

//obd.enterLowPowerMode();
/**
 * Create an instance of Carduino
 */
Carduino_Main::Carduino_Main() {
    Serial.begin(115200);
    sys.begin(1, 1);
    Serial.println("Initializing Carduino...");

    this->accelerometerAgent = new Carduino_Accelerometer();
    this->deviceAgent = new Carduino_Device(&sys);
    this->gpsAgent = new Carduino_GPS(&sys);
    this->motionAgent = new Carduino_Motion(this->gpsAgent,
                                            this->accelerometerAgent);
    this->bluetoothAgent = new Carduino_Bluetooth(this->gpsAgent);
    Serial.println("Done initializing");
}

/**
 * Called every iteration of the arduino run loop
 */
void Carduino_Main::runLoop(void) {

    this->accelerometerAgent->runLoop();
    this->gpsAgent->runLoop();
    this->deviceAgent->runLoop();
    this->motionAgent->runLoop();
    this->bluetoothAgent->runLoop();

}
