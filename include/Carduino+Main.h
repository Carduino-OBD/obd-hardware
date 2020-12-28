#ifndef _CARDUINO_MAIN_H
#define _CARDUINO_MAIN_H
#include <FreematicsPlus.h>
#include "Carduino+Bluetooth.h"
#include "Carduino+GPS.h"
#include "Carduino+Accelerometer.h"

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
    Carduino_Accelerometer *accelerometerAgent;
};

#endif
