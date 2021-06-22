#include <Arduino.h>
#include "Carduino+Bluetooth.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "6a299733-725a-47fe-9df4-fa432b4c210c"
#define CHARACTERISTIC_UUID "847d333c-0a92-41f6-9027-ed2c2be47aa0"

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();

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


/**
 * Create an instance of the bluetooth manager
 */
Carduino_Bluetooth::Carduino_Bluetooth() {
    Serial.println("Initializing Bluetooth...");


    BLEDevice::init("Carduino");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
            CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
                                         );

    pCharacteristic->setCallbacks(new MyCallbacks());

    pCharacteristic->setValue("Hello World says Neil");
    pService->start();
    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(
        0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Bluetooth initialized");
}


void Carduino_Bluetooth::runLoop(void) {

}
