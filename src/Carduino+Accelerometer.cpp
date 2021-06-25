#include <Arduino.h>

#include <FreematicsMEMS.h>
#include "Carduino+Accelerometer.h"

#define STATE_MEMS_READY 0x20
#define STATE_STANDBY 0x80
#define ENABLE_ORIENTATION 1

float accBias[3];
float acc[3] = {0};
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
        delay(10);
    }
    accBias[0] /= n;
    accBias[1] /= n;
    accBias[2] /= n;
    Serial.print("ACC Bias:");
    Serial.print(accBias[0]);
    Serial.print('/');
    Serial.print(accBias[1]);
    Serial.print('/');
    Serial.println(accBias[2]);
}

void standy() {
    if (checkState(STATE_MEMS_READY)) {
        calibrateMEMS();
        while (checkState(STATE_STANDBY)) {
            // calculate relative movement
            float motion = 0;
            for (byte n = 0; n < 10; n++) {
                mems->read(acc);
                for (byte i = 0; i < 3; i++) {
                    float m = (acc[i] - accBias[i]);
                    motion += m * m;
                }
            }
        }
    }
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
    bool updated;
    updated = mems->read(acc, gyr, mag, 0, &ori);
    if (updated) {
        return;
        Serial.print("Accel (x,y,z): ");
        Serial.print(acc[0] * 100, 2);
        Serial.print(' ');
        Serial.print(acc[1] * 100, 2);
        Serial.print(' ');
        Serial.println(acc[2] * 100, 2);
        // store.log(PID_ACC, (int16_t)(acc[0] * 100), (int16_t)(acc[1] * 100),
        //           (int16_t)(acc[2] * 100));
        // store.log(PID_GYRO, (int16_t)(gyr[0] * 100), (int16_t)(gyr[1] * 100),
        //           (int16_t)(gyr[2] * 100));
        // store.log(PID_ORIENTATION, (int16_t)(ori.yaw * 100), (int16_t)(ori.pitch * 100),
        //           (int16_t)(ori.roll * 100));
        // }
        // updated = mems->read(acc, gyr, mag);
        // if (updated) {
        //     // store.log(PID_ACC, (int16_t)(acc[0] * 100), (int16_t)(acc[1] * 100),
        //     //           (int16_t)(acc[2] * 100));
        //     // store.log(PID_GYRO, (int16_t)(gyr[0] * 100), (int16_t)(gyr[1] * 100),
        //     //           (int16_t)(gyr[2] * 100));
        // }

    }
}
