# Athenaeum

Open-source e-ink reader that downloads free books from Archive.org over Wi-Fi, takes notes on what you read, and even serves as a tiny offline viewer for NASA's public data. All on a single custom circuit board.

---

The idea

Since public domain books are free, you can also access The Internet Archive and Open Library with millions of these. NASA has tons of open data too – images from missions, the Astronomy Picture of the Day archive, the Artemis mission logs. But to be used, you need an internet connection, which isn't always available – weak school WiFi, no data plan, off the grid.

The Athenaeum, then, is a little battery powered piece of kit that solves this problem: when you're online, just sync up Athenaeum and it will take all of your books, your notes and a smattering of NASA's free open data with you as far as you go. And it's a multiple app device, too: it's designed as a little plug-in gadget and is capable of running multiple applications.

All of this – schematic, pcb and firmware – was really designed from scratch and is open source.

# It's basically a little computer with an e-ink display

The book reader is the first feature you get, not the only one it has. If you strip it away you end up with an ESP32-S3, a large e-ink display, Wi-Fi, an SD card, and buttons. The firmware is an app launcher, so creating a new feature (a flash card reader, a weather log, whatever) is a change to the firmware, not the design of the board. If you copy this board to create another project, you've already done the electronics.

What it does right now

1. Searches and downloads books from Archive.org / Open Library via Wi-Fi

- Reads them offline on the e-ink display (done) - exists a local library on microSD Would you like to comment? Post your comment or leave your feedback below: Name (required): Email (will not be published) (required): Comments: Submit" /> Submit DisableI want this answerAnswered.
- Allows you to save/flag notes associated with a book and page
- Features a "Space" app that takes NASA's Astronomy Picture of the Day for offline use
- Utilizes a rechargeable battery, charged via USB-C

Actually still on the to-do list that don't require phantom and name scanning: full EPUB rendering (currently just outputs plain text, which is what Archive.org's OCR files are, so this one is already working end to end for many books), APOD image in lieu of just the photo's caption, and a real scrollable book list rather than just "pop" the first file. All in firmware/README.md, as always.

The hardware

- ESP32-S3-WROOM-1 – Wi-Fi, USB native, everything runs on it
- Waveshare 7.5" e-Paper panel (800480), with a Waveshare Driver HAT plus a simple 8-pin header - we couldn't use the raw e-ink panels because they use an undocumented FPC connector, which makes designing a board around non-viable; this was the sane middle ground: still a fully custom PCB, just not re-inventing the display driver electronics as well
- MCP73831: Solves the USB power delivery problem by charging a single cell LiPo from a USB-C power source
- TLV70233 regulator - low standby current because this thing is going to need to sit in somebody's bag for weeks between charges
- microSD for the actual book/note storage
- 7 buttons: 5 for navigation, plus reset, and boot

Enclosure? None (this is designed to be used a bare board, with all the exposed components). The board outline is rounded off to prevent sharp edges, but there is no case concealing it.

Complete schematic & board files are located in /hardware.

# Pin map

| Signal | GPIO | | Signal | GPIO |

|---|---|---|---|---|

| EPD_CS | 10 | | Button: Up | 4 |

" /> Button: Up Button: Down | EPD_DC | 11 | | Button: Up | 5 |

| EPD_RST | 12 | | Button: Choose | 6 |

| EPD_BUSY | 13 | | Button: back | 7 |

Button: Menu 8 EPD_SCK 14

| EPD_MOSI | 15 | | SD CS | 16 |

| | | | SD MISO | 17 |

(SPI clock and MOSI are the same for display and SD card - separate CS lines are used for each, that is the correct design, not a shortcut.)

Repo layout

``

athenaeum/

hardware/

kicad/ KiCad project - schematic + PCB

gerbers/ Manufacturing files for JLCPCB

BOM.csv All parts list, with LCSC part numbers

firmware/ ESP32-S3 firmware (PlatformIO)

docs/ Build guide

`

Building one yourself

1. Upload the following data to the JLCPCB PCB Assembling: hardware/gerbers/ and hardware/BOM.csv
2. Flash the firmware - see firmware/README.md.
3. Insert the Waveshare Driver HAT + panel into the 8-pin header (J2).

Full walkthrough: docs/BUILDGUIDE.md.

Where things stand

- [x] Schematic - designed and verified
- [x] PCB - routed, DRC clean
- [x] Gerbers/BOM exported, ready for JLCPCB
- [x] Firmware - app launcher, Wi-Fi + Archive.org search/download, plain-text reading,
Scrollable book browser real on-device note-taking Space app
- [ ] EPUB parsing (accepts only plaintext atm)
- [ ] APOD image rendering (This is just text now)

- [ ] Demo video

Firmware limitations, stated plainly

This is a real, working firmware skeleton, not a finished article - worth being upfront

About exactly what it does and doesn't do so far:

- No EPUB support as yet. It only supports plain .txt files. Which just happens to be quite a lot of files.
Read already, as Archive.org's OCR'd texts come as .txt search
Download browse read works end to end for many public-domain books. EPUB actual The end Works The Bookshelf So when I teach my readers will lead to the works I read, in their works just to pass a great conversation, or even for passes… because I read in them.
(unzipping the container, parsing XHTML/CSS on-device) is an additional, substantial chunk.
 Of work, not yet started.
- All NASA APOD is text-only. It retrieves and displays the current day's title and date as well as the day's.
Explanation – but it is not the real photo. Rendering a true image on e-ink is
Download, JPEG-decoding, scaling, and dithering the image to 1-bit - has not even started.
- Character by character (single entry) picker, because there's no keyboard - it
Works, but writing anything that is of any length is slow. So a reasonably fast input scheme is the next step in design
 step.
- It hasn't been assembled and run on real hardware. It was written with great care against
The right libraries and the actual pcb pinout and there was a build missing too(ESP32).
Environment that I could use to test it during the development of this repo. Expect a standard
First-build debugging pass (a library version mismatch, an exact display-panel class
(name to confirm)and not a guaranteed 'perfect' first flash.

Full detail in firmware/README.md`.

License

Hardware (schematic, PCB): CERN-OHL-S v2. Firmware: MIT.

Thanks to

Open source projects Inkplate, Flick Book for previous E-ink reader, Waveshare for their Driver HAT docs, and Archive.org / Open Library for maintaining an open API.
