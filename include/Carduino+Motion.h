#ifndef _CARDUINO_MOTION_H
#define _CARDUINO_MOTION_H
#include <FreematicsPlus.h>
#include "Carduino+GPS.h"
#include "Carduino+Accelerometer.h"

class Carduino_Motion {

public:
    /**
     * Create an instance of the Motion manager
     */
    Carduino_Motion(Carduino_GPS *gpsUnit,
                    Carduino_Accelerometer *accelerometerUnit);

    /**
     * Called every iteration of the arduino run loop
     */
    void runLoop(void);
};
#endif
