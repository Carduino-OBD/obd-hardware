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
    this->bluetoothAgent = new Carduino_Bluetooth();
    this->accelerometerAgent = new Carduino_Accelerometer();
    this->gpsAgent = new Carduino_GPS(&sys);
    Serial.println("Done initializing");
}

/**
 * Called every iteration of the arduino run loop
 */
void Carduino_Main::runLoop(void) {
    this->bluetoothAgent->runLoop();
    this->accelerometerAgent->runLoop();
    this->gpsAgent->runLoop();

}
