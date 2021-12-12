#ifndef _CARDUINO_MAIN_H
#define _CARDUINO_MAIN_H
#include "Carduino+Accelerometer.h"
#include "Carduino+Bluetooth.h"
#include "Carduino+Device.h"
#include "Carduino+DriveManager.h"
#include "Carduino+GPS.h"
#include "Carduino+OBD.h"
#include <FreematicsPlus.h>

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

    char serial[22];

  private:
    Carduino_Bluetooth *bluetoothAgent;
    Carduino_GPS *gpsAgent;
    Carduino_Device *deviceAgent;
    Carduino_Accelerometer *accelerometerAgent;
    Carduino_OBD *obdAgent;
    Carduino_DriveManager *driveAgent;
};

#endif
