#pragma once
#include <SPI.h>
#include <SD.h>
#include "config.h"

class Storage {
public:
    bool begin() {
        // SD shares the SPI bus with the display (SCK/MOSI), own CS + MISO
        SPI.begin(PIN_EPD_SCK, PIN_SD_MISO, PIN_EPD_MOSI, PIN_SD_CS);
        if (!SD.begin(PIN_SD_CS)) {
            return false;
        }
        ensureDir(SD_BOOKS_DIR);
        ensureDir(SD_NOTES_DIR);
        ensureDir(SD_CONFIG_DIR);
        return true;
    }

    uint64_t freeSpaceMB() {
        return (SD.totalBytes() - SD.usedBytes()) / (1024ULL * 1024ULL);
    }

private:
    void ensureDir(const char* path) {
        if (!SD.exists(path)) {
            SD.mkdir(path);
        }
    }
};
