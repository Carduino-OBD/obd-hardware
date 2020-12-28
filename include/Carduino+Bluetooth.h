#ifndef _CARDUINO_BLUETOOTH_H
#define _CARDUINO_BLUETOOTH_H


class Carduino_Bluetooth {

public:
    /**
     * Create an instance of the bluetooth manager
     */
    Carduino_Bluetooth();

    /**
     * Called every iteration of the arduino run loop
     */
    void runLoop(void);
};

#endif
