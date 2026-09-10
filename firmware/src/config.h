#pragma once

// ---- e-Paper display (via Waveshare Driver HAT, through J2) ----
#define PIN_EPD_CS    10
#define PIN_EPD_DC    11
#define PIN_EPD_RST   12
#define PIN_EPD_BUSY  13
#define PIN_EPD_SCK   14
#define PIN_EPD_MOSI  15
// Display is write-only over this driver HAT interface - no MISO needed

// ---- microSD card (shares SCK/MOSI with the display, own CS + dedicated MISO) ----
#define PIN_SD_CS     16
#define PIN_SD_MISO   17

// ---- Navigation buttons (active LOW, external 10k pull-ups) ----
#define PIN_BTN_UP      4
#define PIN_BTN_DOWN    5
#define PIN_BTN_SELECT  6
#define PIN_BTN_BACK    7
#define PIN_BTN_MENU    8

// ---- Battery voltage sense (optional, via ADC) ----
#define PIN_BATT_ADC    9

// ---- Display geometry (Waveshare 7.5" panel) ----
#define EPD_WIDTH   800
#define EPD_HEIGHT  480

// ---- Wi-Fi credentials (fill in, or move to a non-committed secrets.h) ----
#define WIFI_SSID     "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// ---- SD card paths ----
#define SD_BOOKS_DIR   "/books"
#define SD_NOTES_DIR   "/notes"
#define SD_CONFIG_DIR  "/config"
