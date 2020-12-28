#include <Arduino.h>
#include "Carduino+GPS.h"
#include <FreematicsPlus.h>

GPS_DATA *gd = 0;
uint32_t lastGPStime = 0;

float acc[3] = {0};
float gyr[3] = {0};
float mag[3] = {0};
ORIENTATION ori = {0};


bool Carduino_GPS::cellSendCommand(const char *cmd, char *buf, int bufsize,
                                   const char *expected = "\r\nOK", unsigned int timeout = 1000) {
    if (cmd) this->sys->xbWrite(cmd);
    memset(buf, 0, bufsize);
    return this->sys->xbReceive(buf, bufsize, timeout, &expected, 1) != 0;
}

void Carduino_GPS::cellUninit() {
    char buf[32];
    cellSendCommand("AT+CPOF\r", buf, sizeof(buf));
}

bool Carduino_GPS::cellInit() {
    char buf[320];
    bool success = false;
    for (byte n = 0; n < 2; n++) {
        if (!cellSendCommand("AT\r", buf, sizeof(buf))) cellSendCommand(0, buf,
                    sizeof(buf), "START", 5000);
        if (cellSendCommand("ATE0\r", buf, sizeof(buf))
                && cellSendCommand("ATI\r", buf, sizeof(buf))) {
            Serial.println(buf);
            cellSendCommand("AT+CVAUXV=61\r", buf, sizeof(buf));
            cellSendCommand("AT+CVAUXS=1\r", buf, sizeof(buf));
            if (cellSendCommand("AT+CGPS?\r", buf, sizeof(buf), "+CGPS: 1")) {
                success = true;
                break;
            }
            delay(2000);
            if (cellSendCommand("AT+CGPS=1,1\r", buf, sizeof(buf))) {
                success = true;
                break;
            }
        }
        this->sys->xbTogglePower();
    }
    //cellSendCommand("AT+CGPSNMEARATE=1\r", buf, sizeof(buf));
    //cellSendCommand("AT+CGPSINFOCFG=1,31\r", buf, sizeof(buf));
    return success;
}


/**
 * Create an instance of the GPS manager
 */
Carduino_GPS::Carduino_GPS(FreematicsESP32 *sysArg) {
    Serial.println("Initializing GPS...");
    this->sys = sysArg;

    Serial.print("CELL GPS:");
    if (cellInit()) {
        Serial.println("OK");
        if (!gd) gd = new GPS_DATA;
        memset(gd, 0, sizeof(GPS_DATA));
    } else {
        Serial.println("NO");
    }
    /*
    ",\"gps\":{\"date\":%u,\"time\":%u,\"lat\":%f,\"lng\":%f,\"alt\":%f,\"speed\":%f,\"sat\":%u,\"sentences\":%u,\"errors\":%u}",
            gd->date, gd->time, gd->lat, gd->lng, gd->alt, gd->speed, gd->sat,
            gd->sentences, gd->errors
     */

    Serial.println("GPS initialized");
}


void Carduino_GPS::runLoop(void) {

}
