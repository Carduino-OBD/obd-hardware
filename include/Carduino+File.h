#ifndef _CARDUINO_SD_H
#define _CARDUINO_SD_H
#include <FS.h>
#include <FreematicsPlus.h>
#include <SD.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <sstream>

class CStorage;

class CStorage {
  public:
    virtual bool init() { return true; }
    virtual void uninit() {}
    virtual void log(uint16_t pid, int value) {
        char buf[24];
        byte len = sprintf(buf, "%X%c%d", pid, m_delimiter, value);
        dispatch(buf, len);
    }
    virtual void log(uint16_t pid, uint32_t value) {
        char buf[24];
        byte len = sprintf(buf, "%X%c%u", pid, m_delimiter, value);
        dispatch(buf, len);
    }
    virtual void log(uint16_t pid, float value) {
        char buf[24];
        byte len = sprintf(buf, "%X%c%f", pid, m_delimiter, value);
        dispatch(buf, len);
    }
    virtual void log(uint16_t pid, float value[]) {
        char buf[48];
        byte len = sprintf(buf, "%X%c%.2f;%.2f;%.2f", pid, m_delimiter, value[0], value[1], value[2]);
        dispatch(buf, len);
    }
    virtual void timestamp(uint32_t ts) { log(0, ts); }
    virtual void purge() { m_samples = 0; }
    virtual uint16_t samples() { return m_samples; }
    virtual void dispatch(const char *buf, size_t len) {
        // output data via serial
        Serial.write((uint8_t *)buf, len);
        m_samples++;
    }

  protected:
    byte checksum(const char *data, int len) {
        byte sum = 0;
        for (int i = 0; i < len; i++)
            sum += data[i];
        return sum;
    }
    virtual void header(const char *devid) {}
    virtual void tailer() {}
    uint16_t m_samples = 0;
    char m_delimiter = ':';
};

class CStorageRAM : public CStorage {
  public:
    bool init(unsigned int cacheSize) {
        if (m_cacheSize != cacheSize) {
            uninit();
            m_cache = new char[m_cacheSize = cacheSize];
        }
        return true;
    }
    void uninit() {
        if (m_cache) {
            delete m_cache;
            m_cache = 0;
            m_cacheSize = 0;
        }
    }
    void purge() {
        m_cacheBytes = 0;
        m_samples = 0;
    }
    unsigned int length() { return m_cacheBytes; }
    char *buffer() { return m_cache; }
    void dispatch(const char *buf, size_t len) {
        // reserve some space for checksum
        int remain = m_cacheSize - m_cacheBytes - len - 3;
        if (remain < 0) {
            // m_cache full
            return;
        }
        // store data in m_cache
        memcpy(m_cache + m_cacheBytes, buf, len);
        m_cacheBytes += len;
        m_samples++;
    }

    void header(const char *devid) { m_cacheBytes = sprintf(m_cache, "%s#", devid); }
    void tailer() {
        // if (m_cache[m_cacheBytes - 1] == ',') m_cacheBytes--;
        m_cacheBytes += sprintf(m_cache + m_cacheBytes, "*%X", (unsigned int)checksum(m_cache, m_cacheBytes));
    }
    void untailer() {
        char *p = strrchr(m_cache, '*');
        if (p) {
            *p = ',';
            m_cacheBytes = p + 1 - m_cache;
        }
    }

  protected:
    unsigned int m_cacheSize = 0;
    unsigned int m_cacheBytes = 0;
    char *m_cache = 0;
};

class FileLogger : public CStorage {
  public:
    FileLogger() { m_delimiter = ','; }
    virtual void dispatch(const char *buf, size_t len) {
        if (!open)
            return;

        if (m_file.write((uint8_t *)buf, len) != len) {
            // try again
            if (m_file.write((uint8_t *)buf, len) != len) {
                Serial.println("Error writing. End file logging.");
                end();
                return;
            }
        }
        m_file.write('\n');
        m_size += (len + 1);
    }
    virtual uint32_t size() { return m_size; }
    virtual void end() {
        m_file.close();
        open = false;
        m_size = 0;
    }
    virtual void flush() { m_file.flush(); }

  protected:
    uint32_t m_dataTime = 0;
    uint32_t m_dataCount = 0;
    uint32_t m_size = 0;
    bool open = false;
    std::string fileName;
    File m_file;
};

class SDLogger : public FileLogger {
  public:
    bool init() {
        SPI.begin();
        if (SD.begin(PIN_SD_CS, SPI, SPI_FREQ)) {
            unsigned int total = SD.totalBytes() >> 20;
            unsigned int used = SD.usedBytes() >> 20;
            Serial.print("SD:");
            Serial.print(total);
            Serial.print(" MB total, ");
            Serial.print(used);
            Serial.println(" MB used");
            return true;
        } else {
            Serial.println("NO SD CARD");
            return false;
        }
    }
    bool begin(std::string fileName) {
        this->fileName = fileName;
        open = true;
        File root = SD.open("/DATA");
        SD.mkdir("/DATA");

        std::stringstream ss;
        ss << "/DATA/" << fileName;
        std::string path = ss.str();

        Serial.print("File: ");
        Serial.println(path.c_str());
        m_file = SD.open(path.c_str(), FILE_WRITE);
        if (!m_file) {
            Serial.println("File error");
            open = false;
            return false;
        }
        m_dataCount = 0;
        return true;
    }
    void flush() {
        std::stringstream ss;
        ss << "/DATA/" << fileName;
        std::string path = ss.str();

        m_file.close();
        m_file = SD.open(path.c_str(), FILE_APPEND);
        if (!m_file) {
            Serial.println("File error");
        }
    }
};

class SPIFFSLogger : public FileLogger {
  public:
    SPIFFSLogger() {}
    int begin(std::string fileName) {
        if (SPIFFS.begin(true)) {
            this->fileName = fileName;
            File root = SPIFFS.open("/");

            std::stringstream ss;
            ss << "/DATA/" << fileName;
            std::string path = ss.str();

            Serial.print("File: ");
            Serial.println(path.c_str());
            m_dataCount = 0;
            m_file = SPIFFS.open(path.c_str(), FILE_WRITE);
            return SPIFFS.totalBytes() - SPIFFS.usedBytes();
        }

        return -1;
    }
    void write(const char *buf, byte len) {
        if (m_file.write((uint8_t *)buf, len) != len) {
            purge();
            if (m_file.write((uint8_t *)buf, len) != len) {
                Serial.println("Error writing data");
                return;
            }
        }
        m_file.write('\n');
    }
};
#endif
