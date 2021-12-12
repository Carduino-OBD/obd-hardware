#include "Carduino+Bluetooth.h"
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID "6a299733-725a-47fe-9df4-fa432b4c210c"
#define MOTION_UUID "d0ba1799-8dfe-4839-bb0f-f0bbb1756781"
#define COMMAND_UUID "847d333c-0a92-41f6-9027-ed2c2be47aa0"

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *commandCharacteristic) {
        std::string value = commandCharacteristic->getValue();

        if (value.length() > 0) {
            Serial.println("*********");
            Serial.print("New value: ");
            for (int i = 0; i < value.length(); i++)
                Serial.print(value[i]);

            Serial.println();
            Serial.println("*********");
        }
    }
};

unsigned long hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

BLECharacteristic *motionCharacteristic;
BLECharacteristic *commandCharacteristic;

/**
 * Create an instance of the bluetooth manager
 */
Carduino_Bluetooth::Carduino_Bluetooth(Carduino_GPS *gpsAgent, char *serial) {

    Serial.println("Initializing Bluetooth...");

    this->gpsAgent = gpsAgent;

    Serial.printf("Pairing code: %lu\n", hash((unsigned char *)serial));

    BLEDevice::init("Carduino");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);

    motionCharacteristic = pService->createCharacteristic(MOTION_UUID, BLECharacteristic::PROPERTY_READ);

    commandCharacteristic = pService->createCharacteristic(COMMAND_UUID, BLECharacteristic::PROPERTY_READ |
                                                                             BLECharacteristic::PROPERTY_WRITE);

    commandCharacteristic->setCallbacks(new MyCallbacks());

    commandCharacteristic->setValue("Hello World says Neil");
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Bluetooth initialized");
}

void Carduino_Bluetooth::runLoop(void) {
    // GPS
    // typedef struct {
    //     uint32_t ts;
    //     uint32_t date;
    //     uint32_t time;
    //     float lat;
    //     float lng;
    //     float alt; /* meter */
    //     float speed; /* knot */
    //     uint16_t heading; /* degree */
    //     uint8_t hdop;
    //     uint8_t sat;
    //     uint16_t sentences;
    //     uint16_t errors;
    // } GPS_DATA;

    // if (lastGPStime == gd->time) return;
    // float kph = gd->speed * 1852 / 1000;

    // Serial.print("[GPS] ");

    // char buf[32];
    // sprintf(buf, "%02u:%02u:%02u.%c",
    //         gd->time / 1000000, (gd->time % 1000000) / 10000, (gd->time % 10000) / 100,
    //         '0' + (gd->time % 100) / 10);
    // Serial.print(buf);

    // Serial.print(' ');
    // Serial.print(gd->lat, 6);
    // Serial.print(' ');
    // Serial.print(gd->lng, 6);
    // Serial.print(' ');
    // Serial.print((int)kph);
    // Serial.print("km/h");
    // if (gd->sat) {
    //     Serial.print(" SATS:");
    //     Serial.print(gd->sat);
    // }
    // Serial.println();

    // lastGPStime = gd->time;

    uint8_t gpsData[21];
    gpsData[0] = this->gpsAgent->gd->time;
    gpsData[1] = this->gpsAgent->gd->time >> 8;
    gpsData[2] = this->gpsAgent->gd->time >> 16;
    gpsData[3] = this->gpsAgent->gd->time >> 24;

    gpsData[4] = 0;
    gpsData[5] = 0;
    gpsData[6] = 0;
    gpsData[7] = 0;
    gpsData[8] = 0;
    gpsData[9] = 0;
    gpsData[10] = 0;
    gpsData[11] = 0;
    gpsData[12] = 0;
    gpsData[13] = 0;
    gpsData[14] = 0;
    gpsData[15] = 0;
    gpsData[16] = 0;
    gpsData[17] = 0;
    gpsData[18] = 0;
    gpsData[19] = 0;
    gpsData[20] = 0;

    motionCharacteristic->setValue(gpsData, 21);
}
