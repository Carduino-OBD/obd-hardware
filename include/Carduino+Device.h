#ifndef _CARDUINO_DEVICE_H
#define _CARDUINO_DEVICE_H
#include <FreematicsPlus.h>

class Carduino_Device {

public:
    /**
     * Create an instance of the Device manager
     */
    Carduino_Device(FreematicsESP32 *sysArg);

    /**
     * Called every iteration of the arduino run loop
     */
    void runLoop(void);

private:
    FreematicsESP32 *sys;

};
#endif
