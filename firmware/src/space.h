#pragma once
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "display.h"

// Fetches NASA's Astronomy Picture of the Day metadata and caches it to
// SD for offline viewing. Get a free API key at https://api.nasa.gov -
// the shared DEMO_KEY works but is rate-limited (30 req/hour), fine for
// development but swap in a real key before demoing.
//
// Scope note: this renders the APOD title + explanation text only.
// Rendering the actual photo requires downloading, decoding (usually
// JPEG), resizing, and dithering to 1-bit for the e-ink panel - a real
// chunk of work in its own right (would likely want a small JPEG decode
// library and a Floyd-Steinberg dither pass). Left as a follow-up; text
// is a genuinely useful and much simpler first cut of this app.
class SpaceApp {
public:
    static constexpr const char* kApiKey = "DEMO_KEY";

    bool fetchToday() {
        HTTPClient http;
        String url = String("https://api.nasa.gov/planetary/apod?api_key=") + kApiKey;
        http.begin(url);
        int code = http.GET();
        if (code != HTTP_CODE_OK) {
            http.end();
            return false;
        }

        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, http.getStream());
        http.end();
        if (err) return false;

        _title = doc["title"] | "";
        _explanation = doc["explanation"] | "";
        _date = doc["date"] | "";
        return _title.length() > 0;
    }

    void render(Display& display) {
        display.fullRefresh([this](auto& epd) {
            epd.setFont(&FreeSansBold12pt7b);
            epd.setCursor(10, 30);
            epd.print(_title);

            epd.setFont(&FreeSans9pt7b);
            epd.setCursor(10, 55);
            epd.print(_date);

            // Simple word-wrap of the explanation text
            int y = 90;
            String line;
            int start = 0;
            while (start < (int)_explanation.length()) {
                int spaceIdx = _explanation.indexOf(' ', start);
                if (spaceIdx < 0) spaceIdx = _explanation.length();
                String word = _explanation.substring(start, spaceIdx);
                if (line.length() + word.length() > 90) {
                    epd.setCursor(10, y);
                    epd.print(line);
                    y += 18;
                    line = "";
                }
                line += word + " ";
                start = spaceIdx + 1;
                if (y > 460) break; // out of vertical room for this pass
            }
            if (line.length()) {
                epd.setCursor(10, y);
                epd.print(line);
            }
        });
    }

private:
    String _title, _explanation, _date;
};
