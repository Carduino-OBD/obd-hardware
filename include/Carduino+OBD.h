#ifndef _CARDUINO_OBD_H
#define _CARDUINO_OBD_H
#include <FreematicsPlus.h>

class Carduino_OBD {

public:
    /**
     * Create an instance of the OBD manager
     */
    Carduino_OBD(FreematicsESP32 *sysArg);

    /**
     * Called every iteration of the arduino run loop
     */
    void runLoop(void);
};
#endif
