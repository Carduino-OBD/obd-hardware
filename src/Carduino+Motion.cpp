#include <Arduino.h>

#include <FreematicsMEMS.h>
#include "Carduino+Motion.h"

/**
 * Create an instance of the acceloremeter manager
 */
Carduino_Motion::Carduino_Motion(Carduino_GPS *gpsUnit,
                                 Carduino_Accelerometer *accelerometerUnit) {
    Serial.println("Initializing Motion unit...");

    Serial.println("Motion unit initialized");
}


void Carduino_Motion::runLoop(void) {

}
