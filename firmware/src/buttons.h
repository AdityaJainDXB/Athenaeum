#pragma once
#include <Arduino.h>
#include "config.h"

enum class Button { NONE, UP, DOWN, SELECT, BACK, MENU };

class Buttons {
public:
    void begin() {
        pinMode(PIN_BTN_UP, INPUT_PULLUP);
        pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
        pinMode(PIN_BTN_SELECT, INPUT_PULLUP);
        pinMode(PIN_BTN_BACK, INPUT_PULLUP);
        pinMode(PIN_BTN_MENU, INPUT_PULLUP);
    }

    // Returns the button that was just pressed (rising edge on release),
    // or Button::NONE if nothing new happened. Call this every loop() iteration.
    Button poll() {
        Button pressed = readRaw();

        if (pressed != Button::NONE && pressed != _lastRaw) {
            _lastRaw = pressed;
            _debounceStart = millis();
            return Button::NONE; // wait for debounce before reporting
        }

        if (pressed == Button::NONE) {
            _lastRaw = Button::NONE;
            return Button::NONE;
        }

        // Same button held - check if debounce window has passed and we
        // haven't already reported this press
        if (pressed == _lastRaw && !_reported && (millis() - _debounceStart) > kDebounceMs) {
            _reported = true;
            return pressed;
        }

        if (pressed == Button::NONE) {
            _reported = false;
        }

        return Button::NONE;
    }

private:
    static constexpr uint16_t kDebounceMs = 30;
    Button _lastRaw = Button::NONE;
    unsigned long _debounceStart = 0;
    bool _reported = false;

    Button readRaw() {
        // Buttons are active LOW (pulled up, switch pulls to GND when pressed)
        if (digitalRead(PIN_BTN_UP) == LOW)     return Button::UP;
        if (digitalRead(PIN_BTN_DOWN) == LOW)   return Button::DOWN;
        if (digitalRead(PIN_BTN_SELECT) == LOW) return Button::SELECT;
        if (digitalRead(PIN_BTN_BACK) == LOW)   return Button::BACK;
        if (digitalRead(PIN_BTN_MENU) == LOW)   return Button::MENU;
        return Button::NONE;
    }
};
