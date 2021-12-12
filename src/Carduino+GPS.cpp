#include "Carduino+GPS.h"
#include "esp_task_wdt.h"
#include <Arduino.h>
#include <FreematicsPlus.h>
#include <TinyGPS.h>

#define GPS_SERIAL_BAUDRATE 115200L

GPS_DATA *gd = 0;
uint32_t lastGPStime = 0;

long parseDegree(const char *s) {
    char *p;
    unsigned long left = atol(s);
    unsigned long tenk_minutes = (left % 100UL) * 100000UL;
    if ((p = strchr(s, '.'))) {
        unsigned long mult = 10000;
        while (isdigit(*++p)) {
            tenk_minutes += mult * (*p - '0');
            mult /= 10;
        }
    }
    return (left / 100) * 1000000 + tenk_minutes / 6;
}

// bool Carduino_GPS::cellSendCommand(const char *cmd, char *buf, int bufsize,
//                                    const char *expected = "\r\nOK", unsigned int timeout = 1000) {
//     if (cmd) this->sys->xbWrite(cmd);
//     memset(buf, 0, bufsize);
//     return this->sys->xbReceive(buf, bufsize, timeout, &expected, 1) != 0;
// }

// void Carduino_GPS::cellUninit() {
//     char buf[32];
//     cellSendCommand("AT+CPOF\r", buf, sizeof(buf));
// }

// bool Carduino_GPS::cellInit() {
//     char buf[320];
//     bool success = false;
//     for (byte n = 0; n < 2; n++) {
//         if (!cellSendCommand("AT\r", buf, sizeof(buf))) cellSendCommand(0, buf,
//                     sizeof(buf), "START", 5000);
//         if (cellSendCommand("ATE0\r", buf, sizeof(buf))
//                 && cellSendCommand("ATI\r", buf, sizeof(buf))) {
//             Serial.println(buf);
//             cellSendCommand("AT+CVAUXV=61\r", buf, sizeof(buf));
//             cellSendCommand("AT+CVAUXS=1\r", buf, sizeof(buf));
//             if (cellSendCommand("AT+CGPS?\r", buf, sizeof(buf), "+CGPS: 1")) {
//                 success = true;
//                 break;
//             }
//             delay(2000);
//             if (cellSendCommand("AT+CGPS=1,1\r", buf, sizeof(buf))) {
//                 success = true;
//                 break;
//             }
//         }
//         this->sys->xbTogglePower();
//     }
//     //cellSendCommand("AT+CGPSNMEARATE=1\r", buf, sizeof(buf));
//     //cellSendCommand("AT+CGPSINFOCFG=1,31\r", buf, sizeof(buf));
//     return success;
// }

// bool Carduino_GPS::cellGetGPSInfo(GPS_DATA *gd) {
//     char *p;
//     char buf[160];
//     if (cellSendCommand("AT+CGPSINFO\r", buf, sizeof(buf), "+CGPSINFO:")) do {
//             Serial.print(buf);
//             if (!(p = strchr(buf, ':'))) break;
//             if (*(++p) == ',') break;
//             gd->lat = parseDegree(p);
//             if (!(p = strchr(p, ','))) break;
//             if (*(++p) == 'S') gd->lat = -gd->lat;
//             if (!(p = strchr(p, ','))) break;
//             gd->lng = parseDegree(++p);
//             if (!(p = strchr(p, ','))) break;
//             if (*(++p) == 'W') gd->lng = -gd->lng;
//             if (!(p = strchr(p, ','))) break;
//             gd->date = atol(++p);
//             if (!(p = strchr(p, ','))) break;
//             gd->time = atol(++p);
//             if (!(p = strchr(p, ','))) break;
//             gd->alt = atoi(++p);
//             if (!(p = strchr(p, ','))) break;
//             gd->speed = atof(++p) * 100;
//             if (!(p = strchr(p, ','))) break;
//             gd->heading = atoi(++p);
//             Serial.print("UTC:");
//             Serial.print(gd->date);
//             Serial.print(' ');
//             Serial.print(gd->time);
//             Serial.print(" LAT:");
//             Serial.print(gd->lat);
//             Serial.print(" LNG:");
//             Serial.println(gd->lng);
//             return true;
//         } while (0);
//     return false;
// }

void logLocationData(GPS_DATA *gd) {
    if (lastGPStime == gd->time)
        return;
    float kph = gd->speed * 1852 / 1000;

    Serial.print("[GPS] ");

    char buf[32];
    sprintf(buf, "%02u:%02u:%02u.%c", gd->time / 1000000, (gd->time % 1000000) / 10000, (gd->time % 10000) / 100,
            '0' + (gd->time % 100) / 10);
    Serial.print(buf);

    Serial.print(" Time: ");
    Serial.print(gd->time);

    Serial.print(" Date: ");
    Serial.print(gd->date);
    Serial.print(' ');

    Serial.print(' ');
    Serial.print(gd->lat, 6);
    Serial.print(' ');
    Serial.print(gd->lng, 6);
    Serial.print(' ');
    Serial.print((int)kph);
    Serial.print("km/h");
    if (gd->sat) {
        Serial.print(" SATS:");
        Serial.print(gd->sat);
    }
    Serial.println();

    lastGPStime = gd->time;
}

/**
 * Create an instance of the GPS manager
 */
Carduino_GPS::Carduino_GPS(FreematicsESP32 *sysArg) {
    Serial.println("Initializing GPS...");
    this->sys = sysArg;

    Serial.print("GPS:");
    if (this->sys->gpsBegin(GPS_SERIAL_BAUDRATE)) {
        Serial.println("OK");
    } else {
        this->sys->gpsEnd();
        Serial.println("NO");
    }

    Serial.println("GPS initialized");
}

void Carduino_GPS::runLoop(void) {
    // this->cellGetGPSInfo(gd);
    this->sys->gpsGetData(&gd);
    logLocationData(gd);
}
