#include <Arduino.h>
#include "Carduino+Device.h"

/**
 * Create an instance of the Device manager
 */
Carduino_Device::Carduino_Device(FreematicsESP32 *sysArg) {
    Serial.println("Initializing Device...");
    this->sys = sysArg;


    Serial.println("Device initialized");
}


void Carduino_Device::runLoop(void) {

}
