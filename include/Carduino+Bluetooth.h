#ifndef _CARDUINO_BLUETOOTH_H
#define _CARDUINO_BLUETOOTH_H
#include "Carduino+GPS.h"

class Carduino_Bluetooth {

  public:
    /**
     * Create an instance of the bluetooth manager
     */
    Carduino_Bluetooth(Carduino_GPS *gpsAgent, char *serial);

    /**
     * Called every iteration of the arduino run loop
     */
    void runLoop(void);

  private:
    Carduino_GPS *gpsAgent;
};

#endif
