#pragma once
#include <Arduino.h>
#include <SD.h>
#include <vector>
#include "config.h"
#include "display.h"

// Reads and paginates plain-text (.txt) books onto the e-ink screen.
//
// Scope note: this handles .txt directly, which covers Archive.org's
// "_djvu.txt" OCR derivative used by ArchiveClient::downloadPlainText().
// True EPUB rendering (unzipping the container, parsing XHTML/CSS, and
// reflowing styled text) is a meaningfully larger job and is left as a
// follow-up - see the repository roadmap. A pragmatic middle ground worth
// considering: server-side or one-time conversion of EPUBs to plain text
// before they reach the device, rather than parsing EPUB on-device.
class Reader {
public:
    static constexpr int kCharsPerLine = 90;   // approx for FreeSans9pt7b at 800px wide
    static constexpr int kLinesPerPage = 28;   // approx for 480px tall, leaving margins

    bool open(const String& path) {
        _file = SD.open(path, FILE_READ);
        if (!_file) return false;
        _path = path;
        _pageStarts.clear();
        _pageStarts.push_back(0);
        _currentPage = 0;
        return true;
    }

    void close() {
        if (_file) _file.close();
    }

    // Renders the current page to the display. Advancing/going back moves
    // a simple byte-offset bookmark per page; pages are computed lazily
    // as the reader moves forward (rather than indexing the whole file
    // up front, which would be slow for large books).
    void renderPage(Display& display) {
        if (!_file) return;
        _file.seek(_pageStarts[_currentPage]);

        display.fullRefresh([this](auto& epd) {
            epd.setFont(&FreeSans9pt7b);
            int y = 24;
            String line;
            int linesDrawn = 0;

            while (linesDrawn < kLinesPerPage && _file.available()) {
                int c = _file.read();
                if (c == '\n' || line.length() >= kCharsPerLine) {
                    epd.setCursor(10, y);
                    epd.print(line);
                    line = "";
                    y += 18;
                    linesDrawn++;
                    if (c != '\n' && c >= 0) line += (char)c;
                } else if (c >= 0) {
                    line += (char)c;
                }
            }
            if (line.length() && linesDrawn < kLinesPerPage) {
                epd.setCursor(10, y);
                epd.print(line);
            }
        });

        // Record where the next page should start, if we haven't already
        if (_currentPage + 1 >= (int)_pageStarts.size()) {
            _pageStarts.push_back(_file.position());
        }
    }

    void nextPage(Display& display) {
        if (_currentPage + 1 < (int)_pageStarts.size() ||
            (_file && _file.position() < _file.size())) {
            _currentPage++;
            renderPage(display);
        }
    }

    void prevPage(Display& display) {
        if (_currentPage > 0) {
            _currentPage--;
            renderPage(display);
        }
    }

    int currentPage() const { return _currentPage; }
    String bookId() const {
        // Derive a stable ID from the filename for notes storage
        int slash = _path.lastIndexOf('/');
        int dot = _path.lastIndexOf('.');
        return _path.substring(slash + 1, dot > slash ? dot : _path.length());
    }

private:
    File _file;
    String _path;
    std::vector<size_t> _pageStarts;
    int _currentPage = 0;
};
