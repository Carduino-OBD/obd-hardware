#ifndef _CARDUINO_DEVICE_H
#define _CARDUINO_DEVICE_H
#include <FreematicsPlus.h>
#include "polylineencoder.h"

class Carduino_Drive {

public:
    /**
     * Create an instance of a drive
     */
    Carduino_Drive(char vin[17], uint8_t startFuelLevel, time_t startTime);

    /**
     * Destructor
     */
    ~Carduino_Drive();

    void addFrame(time_t time, uint8_t gpsSpeed, double latitude,
                  double longitude,
                  int16_t heading, int16_t altitude,
                  uint8_t vehicleSpeed);


    void getDriveData(int8_t **data, size_t *size, uint8_t finalFuelTankLevel);
    time_t startTime;

private:
    gepaf::PolylineEncoder<> encoder;
    int bufferCapacity;
    int bufferSize;
    int8_t *buffer;
    char vin[17];
    uint8_t startFuelLevel;
};
#endif
