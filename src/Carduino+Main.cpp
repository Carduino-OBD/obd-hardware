#include "Carduino+Main.h"
#include "Carduino+Bluetooth.h"
#include <Arduino.h>

//obd.enterLowPowerMode();
/**
 * Create an instance of Carduino
 */
Carduino_Main::Carduino_Main() {
    Serial.begin(115200);
    Serial.println("Initializing Carduino...");
    this->bluetoothAgent = new Carduino_Bluetooth();
    Serial.println("Done initializing...");
}

/**
 * Called every iteration of the arduino run loop
 */
void Carduino_Main::runLoop(void) {
}
