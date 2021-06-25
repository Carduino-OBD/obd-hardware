#ifndef _CARDUINO_ACCELEROMETER_H
#define _CARDUINO_ACCELEROMETER_H


class Carduino_Accelerometer {

public:
    /**
     * Create an instance of the Accelorometer manager
     */
    Carduino_Accelerometer();

    /**
     * Called every iteration of the arduino run loop
     */
    void runLoop(void);

    float getMotion();
private:
    float accel[3];
    float motion;
};

#endif
