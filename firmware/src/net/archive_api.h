#pragma once
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SD.h>
#include "config.h"

// Minimal result record for a book search hit.
struct BookResult {
    String identifier;   // Archive.org item identifier
    String title;
    String creator;
};

class ArchiveClient {
public:
    // Searches Archive.org's advanced search API for public-domain texts
    // matching a free-text query. Returns up to maxResults hits.
    std::vector<BookResult> search(const String& query, int maxResults = 10) {
        std::vector<BookResult> results;

        HTTPClient http;
        String url = "https://archive.org/advancedsearch.php?q=" + urlEncode(query) +
                      "+AND+mediatype:texts&fl[]=identifier&fl[]=title&fl[]=creator" +
                      "&rows=" + String(maxResults) + "&output=json";

        http.begin(url);
        int code = http.GET();
        if (code != HTTP_CODE_OK) {
            http.end();
            return results;
        }

        // Stream-parse rather than loading the whole body as a String,
        // since search responses can be a few KB.
        JsonDocument doc; // ArduinoJson v7 auto-sizes
        DeserializationError err = deserializeJson(doc, http.getStream());
        http.end();
        if (err) return results;

        for (JsonObject doc_ : doc["response"]["docs"].as<JsonArray>()) {
            BookResult r;
            r.identifier = doc_["identifier"] | "";
            r.title = doc_["title"] | "(untitled)";
            r.creator = doc_["creator"] | "Unknown";
            if (r.identifier.length() > 0) results.push_back(r);
        }
        return results;
    }

    // Downloads the plain-text version of a book (Archive.org exposes a
    // "_djvu.txt" derivative for most OCR'd public-domain texts) to SD.
    // Returns true on success. EPUB download follows the same pattern via
    // "identifier.epub" where available - left as a follow-up since it
    // needs an on-device unzip + XHTML parse to actually render.
    bool downloadPlainText(const String& identifier, const String& destPath) {
        HTTPClient http;
        String url = "https://archive.org/download/" + identifier + "/" +
                      identifier + "_djvu.txt";
        http.begin(url);
        int code = http.GET();
        if (code != HTTP_CODE_OK) {
            http.end();
            return false;
        }

        File f = SD.open(destPath, FILE_WRITE);
        if (!f) {
            http.end();
            return false;
        }

        WiFiClient* stream = http.getStreamPtr();
        uint8_t buf[512];
        int total = http.getSize();
        int written = 0;
        while (http.connected() && (total < 0 || written < total)) {
            size_t avail = stream->available();
            if (avail) {
                int c = stream->readBytes(buf, min(avail, sizeof(buf)));
                f.write(buf, c);
                written += c;
            } else {
                delay(1);
            }
        }
        f.close();
        http.end();
        return true;
    }

private:
    String urlEncode(const String& s) {
        String out;
        for (char c : s) {
            if (isalnum(c)) out += c;
            else if (c == ' ') out += '+';
            else {
                char buf[4];
                snprintf(buf, sizeof(buf), "%%%02X", (unsigned char)c);
                out += buf;
            }
        }
        return out;
    }
};
