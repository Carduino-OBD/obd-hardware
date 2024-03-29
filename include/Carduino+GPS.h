#ifndef _CARDUINO_GPS_H
#define _CARDUINO_GPS_H
#include <FreematicsPlus.h>

class Carduino_GPS {

  public:
    /**
     * Create an instance of the GPS manager
     */
    Carduino_GPS(FreematicsESP32 *sysArg);

    /**
     * Called every iteration of the arduino run loop
     */
    void runLoop(void);

    GPS_DATA *gd;

  private:
    FreematicsESP32 *sys;
};
#endif
