#ifndef _CARDUINO_MAIN_H
#define _CARDUINO_MAIN_H
#include <FreematicsPlus.h>
#include "Carduino+Bluetooth.h"
#include "Carduino+GPS.h"
#include "Carduino+Device.h"
#include "Carduino+Accelerometer.h"
#include "Carduino+Motion.h"

class Carduino_Main {

public:
    /**
     * Create an instance of Carduino
     */
    Carduino_Main();

    /**
     * Called every iteration of the arduino run loop
     */
    void runLoop(void);
private:
    Carduino_Bluetooth *bluetoothAgent;
    Carduino_GPS *gpsAgent;
    Carduino_Device *deviceAgent;
    Carduino_Accelerometer *accelerometerAgent;
    Carduino_Motion *motionAgent;
};

#endif
