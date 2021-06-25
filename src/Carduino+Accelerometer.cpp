#include <Arduino.h>

#include <FreematicsMEMS.h>
#include "Carduino+Accelerometer.h"

#define STATE_MEMS_READY 0x20
#define STATE_STANDBY 0x80
#define ENABLE_ORIENTATION 1

float accBias[3];
float gyr[3] = {0};
float mag[3] = {0};
ORIENTATION ori = {0};

byte m_state = 0;

MEMS_I2C *mems = 0;
bool checkState(byte flags) {
    return (m_state & flags) == flags;
}

void calibrateMEMS() {
    // MEMS data collected while sleeping
    accBias[0] = 0;
    accBias[1] = 0;
    accBias[2] = 0;
    int n;
    for (n = 0; n < 100; n++) {
        float acc[3] = {0};
        mems->read(acc);
        accBias[0] += acc[0];
        accBias[1] += acc[1];
        accBias[2] += acc[2];
        delay(1);
    }
    accBias[0] /= n;
    accBias[1] /= n;
    accBias[2] /= n;
}

/**
 * Create an instance of the acceloremeter manager
 */
Carduino_Accelerometer::Carduino_Accelerometer() {
    Serial.println("Initializing Accelerometer...");

    Serial.print("MEMS:");
    mems = new MPU9250;
    byte ret = mems->begin(ENABLE_ORIENTATION);
    if (ret) {
        Serial.println("MPU-9250");
    } else {
        mems->end();
        delete mems;
        mems = new ICM_20948_I2C;
        ret = mems->begin(ENABLE_ORIENTATION);
        if (ret) {
            Serial.println("ICM-20948");
        } else {
            Serial.println("NO");
        }
    }

    Serial.println("Accelerometer initialized");
}


void Carduino_Accelerometer::runLoop(void) {
    mems->read(this->accel, gyr, mag, 0, &ori);

    calibrateMEMS();
    float motion = 0;
    for (byte n = 0; n < 10; n++) {
        mems->read(this->accel);
        for (byte i = 0; i < 3; i++) {
            float m = (this->accel[i] - accBias[i]);
            motion += m * m;
        }
    }
    this->motion = motion;

}

float Carduino_Accelerometer::getMotion() {
    return this->motion;
}
