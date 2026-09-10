#pragma once
#include <Arduino.h>
#include <SD.h>
#include <ArduinoJson.h>
#include "config.h"

// Notes are stored one JSON file per book: /notes/<identifier>.json
// containing an array of {"page": N, "text": "..."} entries.
class Notes {
public:
    bool add(const String& bookId, int page, const String& text) {
        String path = String(SD_NOTES_DIR) + "/" + bookId + ".json";

        JsonDocument doc;
        JsonArray arr;

        if (SD.exists(path)) {
            File f = SD.open(path, FILE_READ);
            deserializeJson(doc, f);
            f.close();
            arr = doc.as<JsonArray>();
            if (arr.isNull()) arr = doc.to<JsonArray>();
        } else {
            arr = doc.to<JsonArray>();
        }

        JsonObject entry = arr.add<JsonObject>();
        entry["page"] = page;
        entry["text"] = text;

        File f = SD.open(path, FILE_WRITE);
        if (!f) return false;
        serializeJson(doc, f);
        f.close();
        return true;
    }

    // Returns the raw JSON array for a book's notes (caller parses/iterates).
    JsonDocument load(const String& bookId) {
        JsonDocument doc;
        String path = String(SD_NOTES_DIR) + "/" + bookId + ".json";
        if (SD.exists(path)) {
            File f = SD.open(path, FILE_READ);
            deserializeJson(doc, f);
            f.close();
        } else {
            doc.to<JsonArray>();
        }
        return doc;
    }
};
