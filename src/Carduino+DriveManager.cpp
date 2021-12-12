#include "Carduino+File.h"
#include <Arduino.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <time.h>

#include "Carduino+Drive.h"
#include "Carduino+DriveManager.h"
#include <FreematicsPlus.h>
#define OBD_RECV_BUF_SIZE 80

// Arduino does not support timegm for some reason, taken from https://stackoverflow.com/q/16647819/3648220
// TODO: move this to its own file
int days_from_civil(int y, int m, int d) {
    y -= m <= 2;
    int era = y / 400;
    int yoe = y - era * 400;                                  // [0, 399]
    int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1; // [0, 365]
    int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;          // [0, 146096]
    return era * 146097 + doe - 719468;
}

time_t timegm(tm const *t) { // It  does not modify broken-down time
    int year = t->tm_year + 1900;
    int month = t->tm_mon; // 0-11
    if (month > 11) {
        year += month / 12;
        month %= 12;
    } else if (month < 0) {
        int years_diff = (11 - month) / 12;
        year -= years_diff;
        month += 12 * years_diff;
    }
    int days_since_1970 = days_from_civil(year, month + 1, t->tm_mday);

    return 60 * (60 * (24L * days_since_1970 + t->tm_hour) + t->tm_min) + t->tm_sec;
}

// END TODO

/**
 * Create an instance of the drive manager
 */
Carduino_DriveManager::Carduino_DriveManager(Carduino_GPS *gpsUnit, Carduino_Accelerometer *accelerometerUnit,
                                             Carduino_OBD *obdUnit) {
    Serial.println("Initializing drive manager unit...");

    this->gpsUnit = gpsUnit;
    this->accelerometerUnit = accelerometerUnit;
    this->obdUnit = obdUnit;

    Serial.println("Drive manager initialized");
}

unsigned long lastSaveTime = 0;
uint32_t lastGPSSavetime = 0;
SDLogger logger;

bool hasSaved = false;
bool hasConnected = false;

Carduino_Drive *currentDrive = NULL;

void Carduino_DriveManager::runLoop(void) {

    // millis() rolls over after 50 days so take the abs
    if (abs(millis() - lastSaveTime) > 7000 && this->obdUnit->isConnected()) {
        if (!hasConnected) {
            hasConnected = true;
        }
        Serial.print("Logging gps");
        if (lastGPSSavetime == this->gpsUnit->gd->time) {
            Serial.println(" No GPS, returning");
            return;
        }

        if (this->gpsUnit->gd->lat == 0 || this->gpsUnit->gd->lng == 0) {
            Serial.println(" GPS coordinates 0, returning");
            return;
        }

        char timeBuf[32];
        sprintf(timeBuf, "%02u:%02u:%02u", this->gpsUnit->gd->time / 1000000,
                (this->gpsUnit->gd->time % 1000000) / 10000, (this->gpsUnit->gd->time % 10000) / 100);

        char dateBuf[32];
        sprintf(dateBuf, "%02u/%02u/%02u", (this->gpsUnit->gd->date % 1000000) / 10000,
                (this->gpsUnit->gd->date % 10000) / 100, this->gpsUnit->gd->date % 100);

        struct std::tm tm;
        std::stringstream ss;
        ss << timeBuf << " " << dateBuf << " +0";
        strptime(ss.str().c_str(), "%H:%M:%S %d/%m/%y %z", &tm);
        std::time_t time = timegm(&tm);

        if (currentDrive == NULL) {
            // TODO: Get the actual fuel level
            currentDrive = new Carduino_Drive(this->obdUnit->vinBuffer, 100, time);
        }

        hasSaved = false;
        lastGPSSavetime = this->gpsUnit->gd->time;

        /*
            void addFrame(uint32_t time, uint8_t gpsSpeed, int32_t latitude,
                  int32_t longitude,
                  int16_t heading, int16_t altitude,
                  uint8_t vehicleSpeed);
         */

        // TODO: get actual vehicle speed
        currentDrive->addFrame(time, this->gpsUnit->gd->speed * 1852 / 1000, this->gpsUnit->gd->lat,
                               this->gpsUnit->gd->lng, this->gpsUnit->gd->heading, this->gpsUnit->gd->alt,
                               this->obdUnit->getVehicleSpeed());

        lastSaveTime = millis();
    }

    if (!this->obdUnit->isConnected() && !hasSaved && hasConnected) {
        Serial.println("Attempting save");
        logger.init();

        char fileName[15];
        sprintf(fileName, "%10lu.cdu", currentDrive->startTime);

        int fileid = logger.begin(fileName);

        int8_t *buffer;
        size_t bufferSize;
        // TODO: Get actual fuel level
        currentDrive->getDriveData(&buffer, &bufferSize, 50);

        logger.dispatch((const char *)buffer, bufferSize);
        logger.flush();
        logger.end();

        free(buffer);

        hasConnected = false;
        hasSaved = true;

        currentDrive = NULL;
    }
}
