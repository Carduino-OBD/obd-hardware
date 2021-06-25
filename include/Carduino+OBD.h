#ifndef _CARDUINO_OBD_H
#define _CARDUINO_OBD_H
#include <FreematicsPlus.h>
#include "Carduino+Accelerometer.h"

class Carduino_OBD {

public:
    /**
     * Create an instance of the OBD manager
     */
    Carduino_OBD(FreematicsESP32 *sysArg,
                 Carduino_Accelerometer *accelerometerUnit);

    /**
     * Called every iteration of the arduino run loop
     */
    void runLoop(void);


    bool isConnected();
private:
    Carduino_Accelerometer *accelerometerUnit;
    bool connected;
};


#endif
