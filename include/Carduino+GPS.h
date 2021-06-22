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

private:
    FreematicsESP32 *sys;

    bool cellSendCommand(const char *cmd, char *buf, int bufsize,
                         const char *expected, unsigned int timeout);
    void cellUninit();
    bool cellInit();
    bool cellGetGPSInfo(GPS_DATA *gd);

    void processGPSData();
    void waitGPS();
};
#endif
