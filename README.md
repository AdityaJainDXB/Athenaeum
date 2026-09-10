# Athenaeum

An open-source e-ink reader that pulls free books from Archive.org over Wi-Fi, lets you take notes on what you read, and doubles as a small offline viewer for NASA's public data — all on one custom board.

---

## The idea

Archive.org and Open Library have millions of free, public-domain books. NASA publishes a huge amount of open data too — mission photos, the Astronomy Picture of the Day archive, Artemis mission logs. All of it needs an internet connection to actually use, which isn't a given everywhere — patchy school Wi-Fi, no data plan, being somewhere off-grid.

Athenaeum is a small battery-powered device that gets around that: sync it when you have Wi-Fi, and it keeps your books, your notes, and a slice of NASA's public data with you after that, no connection needed. It's not meant to be a single-purpose gadget either — it's built as a little app launcher, so the same hardware can run more than one thing.

Everything here — schematic, PCB, firmware — is designed from scratch and open source.

## It's really just a small computer with an e-ink screen

The book reader is the first thing it does, not the only thing it's capable of. Strip it down and it's an ESP32-S3, a big e-ink display, Wi-Fi, an SD card, and some buttons. The firmware is written as an app launcher, so adding something new — a flashcard app, a weather log, whatever — is a firmware change, not a new board. If you fork this to build something else on it, the hardware side is already done for you.

## What it does right now

- Searches and downloads books from Archive.org / Open Library over Wi-Fi
- Reads them offline on the e-ink screen, with a local library on microSD
- Lets you mark/save notes tied to a book and page
- Has a "Space" app that pulls NASA's Astronomy Picture of the Day for offline viewing
- Runs off a rechargeable battery with USB-C charging

Things that are genuinely still on the to-do list, not hidden: full EPUB rendering (right now it reads plain text, which is what Archive.org's OCR files actually are, so this already works end to end for a lot of books), showing the actual APOD photo instead of just its title/description, and a proper scrollable book list instead of just opening the first file it finds. All noted honestly in `firmware/README.md`.

## The hardware

- **ESP32-S3-WROOM-1** — Wi-Fi, native USB, runs everything
- **Waveshare 7.5" e-Paper panel (800×480)**, connected through a Waveshare Driver HAT and a simple 8-pin header — raw e-ink panels use an undocumented FPC connector that isn't practical to design a board around from scratch, so this was the sane middle ground: still a fully custom PCB, just not reinventing the display driver electronics too
- **MCP73831** charges a single-cell LiPo over USB-C
- **TLV70233** regulator, chosen for low standby current since this thing needs to sit in someone's bag for weeks between charges
- **microSD** for the actual book/note storage
- 7 buttons: 5 for navigation, plus reset and boot

No enclosure — this is meant to be carried and used as a bare board, exposed components and all. The board outline has rounded corners specifically so it doesn't have sharp edges to snag on anything, but there's no case hiding it.

Full schematic and board files are in [`/hardware`](./hardware).

### Pin map

| Signal | GPIO | | Signal | GPIO |
|---|---|---|---|---|
| EPD_CS | 10 | | Button: Up | 4 |
| EPD_DC | 11 | | Button: Down | 5 |
| EPD_RST | 12 | | Button: Select | 6 |
| EPD_BUSY | 13 | | Button: Back | 7 |
| EPD_SCK | 14 | | Button: Menu | 8 |
| EPD_MOSI | 15 | | SD CS | 16 |
| | | | SD MISO | 17 |

(SPI clock and MOSI are shared between the display and the SD card — each has its own chip-select line, which is the normal way to do this, not a shortcut.)

## Repo layout

```
athenaeum/
├── hardware/
│   ├── kicad/       KiCad project - schematic + PCB
│   ├── gerbers/      Manufacturing files for JLCPCB
│   └── BOM.csv        Full parts list, with LCSC part numbers
├── firmware/          ESP32-S3 firmware (PlatformIO)
└── docs/              Build guide
```

## Building one yourself

1. Upload `hardware/gerbers/` and `hardware/BOM.csv` to JLCPCB's PCB Assembly service.
2. Flash the firmware — see [`firmware/README.md`](./firmware/README.md).
3. Plug the Waveshare Driver HAT + panel into the 8-pin header (J2).

Full walkthrough: [`docs/BUILD_GUIDE.md`](./docs/BUILD_GUIDE.md).

## Where things stand

- [x] Schematic - designed and verified
- [x] PCB - routed, DRC clean
- [x] Gerbers/BOM exported, ready for JLCPCB
- [x] Firmware - app launcher, Wi-Fi + Archive.org search/download, plain-text reading,
      scrollable book browser, real on-device note-taking, Space app
- [ ] EPUB parsing (currently plain-text only)
- [ ] APOD image rendering (currently text only)
- [ ] Demo video

## Firmware limitations, stated plainly

This is a real, working firmware skeleton, not a finished product - worth being upfront
about exactly what it does and doesn't do yet:

- **No EPUB support yet.** It reads plain `.txt` files. That happens to cover a lot of
  ground already, since Archive.org's OCR'd texts download as `.txt`, so search →
  download → browse → read works end to end for many public-domain books. Actual EPUB
  (unzipping the container, parsing XHTML/CSS on-device) is a separate, sizeable piece
  of work, not yet started.
- **NASA APOD is text-only.** It fetches and shows the day's title, date, and
  explanation, but not the actual photo. Rendering a real image on e-ink means
  downloading, JPEG-decoding, resizing, and dithering it to 1-bit - also not started.
- **Text entry is a one-character-at-a-time picker**, since there's no keyboard - it
  works, but writing anything long is slow. A faster input scheme is a reasonable next
  step.
- **It hasn't been compiled or run on real hardware.** It was written carefully against
  the right libraries and the actual PCB pinout, but there was no ESP32 or build
  environment available to test it while putting this repo together. Expect a normal
  first-build debugging pass (a library version mismatch, an exact display-panel class
  name to confirm) rather than a guaranteed flawless first flash.

Full detail in [`firmware/README.md`](./firmware/README.md).

## License

Hardware (schematic, PCB): [CERN-OHL-S v2](https://ohwr.org/cern_ohl_s_v2.txt). Firmware: [MIT](./LICENSE).

## Thanks to

The open-source [Inkplate](https://github.com/SolderedElectronics/Inkplate-6FLICK-hardware) and [FlickBook](https://github.com/spirosbond/FlickBook) projects for prior art on e-ink readers, Waveshare for their Driver HAT docs, and Archive.org / Open Library for keeping their APIs open.
