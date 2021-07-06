#ifndef _CARDUINO_OBD_H
#define _CARDUINO_OBD_H
#include <FreematicsPlus.h>
#include "Carduino+Accelerometer.h"

#define OBD_RECV_BUF_SIZE 80

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

    char vinBuffer[OBD_RECV_BUF_SIZE + 1];

    uint8_t getVehicleSpeed();
private:
    Carduino_Accelerometer *accelerometerUnit;
    bool connected;
    uint8_t vehicleSpeed;
};


#endif
