#pragma once
#include <Arduino.h>
#include <SD.h>
#include <vector>
#include "config.h"
#include "display.h"

// Lists .txt files in /books and lets the user scroll through them with
// Up/Down and pick one with Select - replaces "just open the first file
// found" from the earlier version.
class BookBrowser {
public:
    void refresh() {
        _files.clear();
        File dir = SD.open(SD_BOOKS_DIR);
        if (!dir) return;

        File f = dir.openNextFile();
        while (f) {
            if (!f.isDirectory()) {
                String name = String(f.name());
                if (name.endsWith(".txt")) {
                    _files.push_back(name);
                }
            }
            f = dir.openNextFile();
        }
        dir.close();
        _selection = 0;
    }

    bool empty() const { return _files.empty(); }
    int count() const { return _files.size(); }

    void moveUp() {
        if (_files.empty()) return;
        _selection = (_selection - 1 + _files.size()) % _files.size();
    }

    void moveDown() {
        if (_files.empty()) return;
        _selection = (_selection + 1) % _files.size();
    }

    String selectedPath() const {
        if (_files.empty()) return "";
        return String(SD_BOOKS_DIR) + "/" + _files[_selection];
    }

    void render(Display& display) {
        display.fullRefresh([this](auto& epd) {
            epd.setFont(&FreeSansBold12pt7b);
            epd.setCursor(10, 30);
            epd.print("Your Books");

            epd.setFont(&FreeSans9pt7b);
            if (_files.empty()) {
                epd.setCursor(20, 80);
                epd.print("No books yet - sync over Wi-Fi first.");
                return;
            }

            // Show a window of up to 10 entries centered around the
            // current selection, so long libraries still scroll sanely
            int windowSize = 10;
            int start = max(0, _selection - windowSize / 2);
            int end = min((int)_files.size(), start + windowSize);

            int y = 70;
            for (int i = start; i < end; i++) {
                epd.setCursor(30, y);
                epd.print(i == _selection ? "> " : "  ");
                // Strip the .txt extension for display
                String title = _files[i];
                title.replace(".txt", "");
                epd.print(title);
                y += 30;
            }
        });
    }

private:
    std::vector<String> _files;
    int _selection = 0;
};
