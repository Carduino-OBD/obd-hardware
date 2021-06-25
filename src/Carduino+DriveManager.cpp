#include <Arduino.h>
#include "Carduino+SD.h"
#include <sstream>
#include <ctime>
#include <iomanip>
#include <time.h>

#include <FreematicsPlus.h>
#include "Carduino+DriveManager.h"
#include "polylineencoder.h"
#define OBD_RECV_BUF_SIZE 80


// Arduino does not support timegm for some reason, taken from https://stackoverflow.com/q/16647819/3648220
// TODO: move this to its own file
int days_from_civil(int y, int m, int d) {
    y -= m <= 2;
    int era = y / 400;
    int yoe = y - era * 400;                                   // [0, 399]
    int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;  // [0, 365]
    int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;           // [0, 146096]
    return era * 146097 + doe - 719468;
}

time_t timegm(tm const *t) {   // It  does not modify broken-down time
    int year = t->tm_year + 1900;
    int month = t->tm_mon;          // 0-11
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
Carduino_DriveManager::Carduino_DriveManager(Carduino_GPS *gpsUnit,
        Carduino_Accelerometer *accelerometerUnit, Carduino_OBD *obdUnit) {
    Serial.println("Initializing drive manager unit...");

    this->gpsUnit = gpsUnit;
    this->accelerometerUnit = accelerometerUnit;
    this->obdUnit = obdUnit;

    Serial.println("Drive manager initialized");
}

unsigned long lastSaveTime = 0;
uint32_t lastGPSSavetime = 0;

gepaf::PolylineEncoder<> encoder;
SDLogger logger;

bool hasSaved = false;
bool hasConnected = false;

void Carduino_DriveManager::runLoop(void) {

    char timeBuf[32];
    sprintf(timeBuf, "%02u:%02u:%02u",
            this->gpsUnit->gd->time / 1000000, (this->gpsUnit->gd->time % 1000000) / 10000,
            (this->gpsUnit->gd->time % 10000) / 100);

    char dateBuf[32];
    sprintf(dateBuf, "%02u/%02u/%02u", (this->gpsUnit->gd->date % 1000000) / 10000,
            (this->gpsUnit->gd->date % 10000) / 100, this->gpsUnit->gd->date % 100);

    struct std::tm tm;
    std::stringstream ss;
    ss << timeBuf << " " << dateBuf << " +0";
    strptime(ss.str().c_str(), "%H:%M:%S %d/%m/%y %z", &tm);
    Serial.println(ss.str().c_str());
    std::time_t time = timegm(&tm);
    return;

    //millis() rolls over after 50 days so take the abs
    if(abs(millis() - lastSaveTime) > 7000 && this->obdUnit->isConnected()) {
        if (!hasConnected) {
            hasConnected = true;
        }
        Serial.print("Logging gps");
        if (lastGPSSavetime == this->gpsUnit->gd->time) {
            Serial.println(" No GPS, returning");
            return;
        }
        hasSaved = false;
        lastGPSSavetime = this->gpsUnit->gd->time;

        encoder.addPoint(this->gpsUnit->gd->lat, this->gpsUnit->gd->lng);

        lastSaveTime = millis();
    }

    if(!this->obdUnit->isConnected() && !hasSaved) {
        Serial.println("Attempting save");
        logger.init();
        int fileid = logger.begin("name");

        std::string res = encoder.encode();
        encoder.clear();

        logger.dispatch(res.c_str(), res.length());
        logger.flush();
        logger.end();
        hasConnected = false;
        hasSaved = true;
    }

}
