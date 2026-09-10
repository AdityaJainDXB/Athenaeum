#pragma once
#include <GxEPD2_BW.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include "config.h"

// Waveshare 7.5" (800x480) monochrome panel via the Universal Driver HAT.
// GxEPD2_750_T7 matches this panel family - swap if your exact revision differs.
class Display {
public:
    Display()
        : epd(GxEPD2_750_T7(PIN_EPD_CS, PIN_EPD_DC, PIN_EPD_RST, PIN_EPD_BUSY)) {}

    void begin() {
        epd.init(115200, true, 2, false);
        epd.setRotation(0);
        epd.setTextColor(GxEPD_BLACK);
    }

    // Full refresh - slower, but ghost-free. Use for page turns / screen changes.
    void fullRefresh(void (*drawCallback)(GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT>&)) {
        epd.setFullWindow();
        epd.firstPage();
        do {
            epd.fillScreen(GxEPD_WHITE);
            drawCallback(epd);
        } while (epd.nextPage());
    }

    void sleep() {
        epd.hibernate();
    }

    void drawText(int16_t x, int16_t y, const char* text, const GFXfont* font = &FreeSans9pt7b) {
        epd.setFont(font);
        epd.setCursor(x, y);
        epd.print(text);
    }

    GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> epd;
};
