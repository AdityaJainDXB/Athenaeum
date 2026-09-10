# Firmware

ESP32-S3 firmware, built on the Arduino core via PlatformIO.

## Structure

```
firmware/
├── platformio.ini          Build config, board target, library dependencies
└── src/
    ├── config.h             Pin definitions (matches the PCB design) + Wi-Fi credentials
    ├── buttons.h             Debounced button reading (Up/Down/Select/Back/Menu)
    ├── display.h              GxEPD2 wrapper for the Waveshare 7.5" panel
    ├── sd_storage.h            microSD init + book/notes/config directory setup
    ├── book_browser.h           Scrollable list of books on the SD card
    ├── reader.h                 Plain-text book reader with pagination
    ├── text_entry.h              On-device character-picker text entry for notes
    ├── notes.h                    Per-book note/highlight storage (JSON on SD)
    ├── space.h                     NASA APOD viewer
    ├── net/archive_api.h            Archive.org search + plain-text download client
    └── main.cpp                     App launcher tying everything together
```

## Building

1. Install [PlatformIO](https://platformio.org/) (CLI or the VS Code extension).
2. Fill in your Wi-Fi credentials in `src/config.h` - it ships with placeholder
   text (`YOUR_WIFI_SSID` etc.), so Wi-Fi-dependent features won't work until
   you swap those for your real network details.
3. Get a free NASA API key at [api.nasa.gov](https://api.nasa.gov) and set it in
   `src/space.h` (the shared `DEMO_KEY` works for development but is rate-limited).
4. `pio run -t upload` - flashes over USB-C (native USB on the ESP32-S3, no
   separate programmer needed).

## What's implemented

- App launcher menu (Reader / Space / Notes / Wi-Fi Sync), navigated with the 5 nav buttons
- **Book browser** - scrolls through every `.txt` file on the SD card, not just the first one found
- Plain-text book reading with pagination, from SD card
- **Real note-taking** - an on-device character picker (Up/Down to change letter, Select to
  add it, Back to delete, Menu to save) so notes are actually typed text, not a placeholder string
- NASA APOD: fetches and displays today's title, date, and explanation text
- Archive.org search (by query) and plain-text download of a result to SD

## Known scope limits (honest, not hidden)

- **EPUB rendering isn't implemented.** The reader and browser both work with plain `.txt`
  files, which conveniently covers Archive.org's `_djvu.txt` OCR derivative used by the
  download client - so search -> download -> browse -> read works end to end for a lot of
  public-domain texts already. True EPUB support means unzipping the container and parsing
  XHTML/CSS on-device, a real chunk of work on its own, tracked as a follow-up.
- **APOD image rendering isn't implemented.** Currently shows the day's title, date, and
  explanation as text only. The actual photo needs JPEG decode + resize + dither-to-1-bit
  for the e-ink panel, also tracked as a follow-up.
- **Text entry is slow by design** - a single-character-at-a-time picker is the realistic
  option on a 5-button device with no keyboard. It works, but writing a long note takes a
  while; a faster input scheme (e.g. T9-style grouping) is a reasonable future improvement.

## A note on testing

This firmware was written and reasoned through carefully, but it has not been compiled or
run on real hardware as part of putting this repository together - there was no ESP32 or
build environment available to do that. Treat the first build as a normal first build:
expect to work through a handful of small compile issues (a library version mismatch, an
exact class name for your specific panel revision) rather than a flawless first flash.
The structure and logic are sound; the fine details of a first bring-up are still yours to
work through.
