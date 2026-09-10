#pragma once
#include <Arduino.h>
#include "display.h"

// Simple on-device text entry for devices with only 5 buttons and no
// keyboard: scroll through a character set with Up/Down, Select to add
// the current character to the note, Menu to finish, Back to delete the
// last character. Not fast, but genuinely lets you write a real note
// rather than saving a fixed placeholder string.
class TextEntry {
public:
    static constexpr const char* kCharset =
        "abcdefghijklmnopqrstuvwxyz0123456789 .,!?'-";
    static constexpr int kCharsetLen = 44; // strlen(kCharset)

    void reset() {
        _text = "";
        _cursorChar = 0;
    }

    void charUp() {
        _cursorChar = (_cursorChar - 1 + kCharsetLen) % kCharsetLen;
    }

    void charDown() {
        _cursorChar = (_cursorChar + 1) % kCharsetLen;
    }

    void confirmChar() {
        _text += kCharset[_cursorChar];
    }

    void backspace() {
        if (_text.length() > 0) {
            _text.remove(_text.length() - 1);
        }
    }

    const String& text() const { return _text; }

    void render(Display& display) {
        display.fullRefresh([this](auto& epd) {
            epd.setFont(&FreeSansBold12pt7b);
            epd.setCursor(10, 30);
            epd.print("New note");

            epd.setFont(&FreeSans9pt7b);
            epd.setCursor(10, 70);
            epd.print(_text);
            epd.print("_"); // cursor marker

            // Show the current character picker position
            epd.setCursor(10, 120);
            epd.print("Char: ");
            epd.print(kCharset[_cursorChar]);
            epd.setCursor(10, 150);
            epd.print("Up/Down: change letter  Select: add  Menu: done  Back: delete");
        });
    }

private:
    String _text;
    int _cursorChar = 0;
};
