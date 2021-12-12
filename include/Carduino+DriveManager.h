#ifndef _CARDUINO_DRIVEMANAGER_H
#define _CARDUINO_DRIVEMANAGER_H
#include "Carduino+Accelerometer.h"
#include "Carduino+GPS.h"
#include "Carduino+OBD.h"
#include <FreematicsPlus.h>

class Carduino_DriveManager {

  public:
    /**
     * Create an instance of the Drive manager
     */
    Carduino_DriveManager(Carduino_GPS *gpsUnit, Carduino_Accelerometer *accelerometerUnit, Carduino_OBD *obdUnit);

    /**
     * Called every iteration of the arduino run loop
     */
    void runLoop(void);

  private:
    Carduino_GPS *gpsUnit;
    Carduino_Accelerometer *accelerometerUnit;
    Carduino_OBD *obdUnit;
};
#endif
