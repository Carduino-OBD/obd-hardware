#include "Carduino+Drive.h"

const int HEADER_SIZE = 28;
const int DRIVE_FRAME_SIZE = 14;
const int FOOTER_SIZE = 1;
const uint16_t VERSION = 1;

Carduino_Drive::Carduino_Drive(char vin[], uint8_t startFuelLevel, time_t startTime) {
    this->bufferCapacity = DRIVE_FRAME_SIZE * 200; // Start with 200 drive frames
    this->bufferSize = 0;

    this->buffer = (int8_t *)(malloc(this->bufferCapacity));

    memcpy(this->vin, vin, 17);

    this->startFuelLevel = startFuelLevel;
    this->startTime = startTime;
}

Carduino_Drive::~Carduino_Drive() { free(this->buffer); }

void Carduino_Drive::addFrame(time_t time, uint8_t gpsSpeed, double latitude, double longitude, int16_t heading,
                              int16_t altitude, uint8_t vehicleSpeed) {
    this->encoder.addPoint(latitude, longitude);
    /*
    |-----GPS Time-----|-----GPS Speed-----|-----Heading-----|-----Altitude-----|-----Vehicle Speed-----|
      uint64_t        uint8_t              int16_t          int16_t              uint8_t
     */

    if (this->bufferCapacity == this->bufferSize) {
        // Allocate another 200 drive frames
        int newCapacity = this->bufferCapacity + DRIVE_FRAME_SIZE * 200;
        this->buffer = (int8_t *)(realloc(this->buffer, newCapacity));
        this->bufferCapacity = newCapacity;
    }

    for (int i = 0; i < 8; i++) {
        if (i >= sizeof(time)) {
            this->buffer[this->bufferSize + i] = 0;
        } else {
            this->buffer[this->bufferSize + i] = (uint8_t)(time >> (i * 8)) & 0xff;
        }
    }

    this->buffer[this->bufferSize + 8] = gpsSpeed;

    this->buffer[this->bufferSize + 9] = (uint8_t)(heading & 0xff);
    this->buffer[this->bufferSize + 10] = (uint8_t)(heading >> 8);

    this->buffer[this->bufferSize + 11] = (uint8_t)(altitude & 0xff);
    this->buffer[this->bufferSize + 12] = (uint8_t)(altitude >> 8);

    this->buffer[this->bufferSize + 13] = vehicleSpeed;

    this->bufferSize += DRIVE_FRAME_SIZE;

    Serial.print(" (");
    Serial.print(latitude);
    Serial.print(",");
    Serial.print(longitude);
    Serial.print(")");
    Serial.println();
}

void Carduino_Drive::getDriveData(int8_t **data, size_t *size, uint8_t finalFuelTankLevel) {

    std::string polyline = this->encoder.encode();
    uint32_t polyLineCount = polyline.length();

    size_t dataSize = HEADER_SIZE + polyLineCount + this->bufferSize + FOOTER_SIZE;

    *data = (int8_t *)(malloc(dataSize));
    *size = dataSize;

    data[0][0] = (uint8_t)(VERSION & 0xff);
    data[0][1] = (uint8_t)(VERSION >> 8);

    memcpy((data[0] + 2), this->vin, 17);

    data[0][19] = this->startFuelLevel;

    data[0][20] = (uint8_t)(polyLineCount & 0xff);
    data[0][21] = (uint8_t)(polyLineCount >> 8) & 0xff;
    data[0][22] = (uint8_t)(polyLineCount >> 16) & 0xff;
    data[0][23] = (uint8_t)(polyLineCount >> 24);

    data[0][24] = (uint8_t)(this->bufferSize & 0xff);
    data[0][25] = (uint8_t)(this->bufferSize >> 8) & 0xff;
    data[0][26] = (uint8_t)(this->bufferSize >> 16) & 0xff;
    data[0][27] = (uint8_t)(this->bufferSize >> 24);

    memcpy((data[0] + 28), polyline.c_str(), polyLineCount);
    memcpy((data[0] + 28 + polyLineCount), this->buffer, this->bufferSize);

    data[0][28 + this->bufferSize + polyLineCount] = finalFuelTankLevel;
}
