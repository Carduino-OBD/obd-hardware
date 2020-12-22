#ifndef _CARDUINO_MAIN_H
#define _CARDUINO_MAIN_H
#include "Carduino+Bluetooth.h"

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
private:
    Carduino_Bluetooth *bluetoothAgent;
};

#endif
