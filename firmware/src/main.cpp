#include <Arduino.h>
#include <WiFi.h>
#include <SD.h>
#include "config.h"
#include "buttons.h"
#include "display.h"
#include "sd_storage.h"
#include "notes.h"
#include "reader.h"
#include "space.h"
#include "book_browser.h"
#include "text_entry.h"
#include "net/archive_api.h"

// --- App launcher state ---
enum class App { MENU, BROWSER, READER, NOTE_ENTRY, SPACE, NOTES };
App currentApp = App::MENU;
int menuSelection = 0;
const char* menuItems[] = { "Reader", "Space", "Notes", "Wi-Fi Sync" };
const int menuItemCount = 4;

Buttons buttons;
Display display;
Storage storage;
Notes notes;
Reader reader;
SpaceApp spaceApp;
ArchiveClient archive;
BookBrowser bookBrowser;
TextEntry textEntry;

bool wifiConnected = false;

void connectWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
        delay(200);
    }
    wifiConnected = (WiFi.status() == WL_CONNECTED);
}

void drawMenu() {
    display.fullRefresh([](auto& epd) {
        epd.setFont(&FreeSansBold12pt7b);
        epd.setCursor(10, 30);
        epd.print("Athenaeum");

        epd.setFont(&FreeSans9pt7b);
        for (int i = 0; i < menuItemCount; i++) {
            epd.setCursor(30, 90 + i * 40);
            if (i == menuSelection) epd.print("> ");
            else epd.print("  ");
            epd.print(menuItems[i]);
        }
    });
}

// ---- Menu ----
void openSelectedApp() {
    switch (menuSelection) {
        case 0:
            bookBrowser.refresh();
            currentApp = App::BROWSER;
            bookBrowser.render(display);
            break;
        case 1:
            currentApp = App::SPACE;
            if (wifiConnected && spaceApp.fetchToday()) {
                spaceApp.render(display);
            }
            break;
        case 2:
            currentApp = App::NOTES;
            break;
        case 3:
            connectWiFi();
            drawMenu();
            break;
    }
}

void handleMenuInput(Button b) {
    switch (b) {
        case Button::UP:
            menuSelection = (menuSelection - 1 + menuItemCount) % menuItemCount;
            drawMenu();
            break;
        case Button::DOWN:
            menuSelection = (menuSelection + 1) % menuItemCount;
            drawMenu();
            break;
        case Button::SELECT:
            openSelectedApp();
            break;
        default:
            break;
    }
}

// ---- Book browser ----
void handleBrowserInput(Button b) {
    switch (b) {
        case Button::UP:
            bookBrowser.moveUp();
            bookBrowser.render(display);
            break;
        case Button::DOWN:
            bookBrowser.moveDown();
            bookBrowser.render(display);
            break;
        case Button::SELECT:
            if (!bookBrowser.empty() && reader.open(bookBrowser.selectedPath())) {
                currentApp = App::READER;
                reader.renderPage(display);
            }
            break;
        case Button::BACK:
            currentApp = App::MENU;
            drawMenu();
            break;
        default:
            break;
    }
}

// ---- Reader ----
void handleReaderInput(Button b) {
    switch (b) {
        case Button::DOWN:
            reader.nextPage(display);
            break;
        case Button::UP:
            reader.prevPage(display);
            break;
        case Button::BACK:
            reader.close();
            currentApp = App::MENU;
            drawMenu();
            break;
        case Button::SELECT:
            // Open real text entry instead of saving a fixed placeholder
            textEntry.reset();
            currentApp = App::NOTE_ENTRY;
            textEntry.render(display);
            break;
        default:
            break;
    }
}

// ---- Note text entry ----
void handleNoteEntryInput(Button b) {
    switch (b) {
        case Button::UP:
            textEntry.charUp();
            textEntry.render(display);
            break;
        case Button::DOWN:
            textEntry.charDown();
            textEntry.render(display);
            break;
        case Button::SELECT:
            textEntry.confirmChar();
            textEntry.render(display);
            break;
        case Button::BACK:
            textEntry.backspace();
            textEntry.render(display);
            break;
        case Button::MENU:
            // Save and return to the reader
            if (textEntry.text().length() > 0) {
                notes.add(reader.bookId(), reader.currentPage(), textEntry.text());
            }
            currentApp = App::READER;
            reader.renderPage(display);
            break;
    }
}

void handleGlobalBack(Button b) {
    if (b == Button::BACK) {
        currentApp = App::MENU;
        drawMenu();
    }
}

void setup() {
    Serial.begin(115200);
    buttons.begin();
    display.begin();

    if (!storage.begin()) {
        display.fullRefresh([](auto& epd) {
            epd.setFont(&FreeSans9pt7b);
            epd.setCursor(10, 30);
            epd.print("SD card not found - insert a card and restart.");
        });
        return;
    }

    drawMenu();
}

void loop() {
    Button b = buttons.poll();
    if (b == Button::NONE) {
        delay(20);
        return;
    }

    switch (currentApp) {
        case App::MENU:
            handleMenuInput(b);
            break;
        case App::BROWSER:
            handleBrowserInput(b);
            break;
        case App::READER:
            handleReaderInput(b);
            break;
        case App::NOTE_ENTRY:
            handleNoteEntryInput(b);
            break;
        case App::SPACE:
        case App::NOTES:
            handleGlobalBack(b);
            break;
    }
}
