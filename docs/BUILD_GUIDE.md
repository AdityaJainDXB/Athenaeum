# Build Guide

## 1. Get the PCB made

1. Open `hardware/kicad/Athenaeum.kicad_pro` in KiCad to review the design, or go straight to manufacturing.
2. Upload the Gerber ZIP from `hardware/gerbers/` to [JLCPCB](https://jlcpcb.com).
3. On JLCPCB's order page, enable **PCB Assembly (PCBA)** and upload `hardware/BOM.csv` and the matching CPL (component placement) file from `hardware/gerbers/`.
4. Confirm the ESP32-S3-WROOM-1, MCP73831, and TLV70233 all matched correctly against JLCPCB's parts library (they're listed as Basic Parts with LCSC numbers in the BOM) — connectors and switches will show as Extended Parts, which is expected.
5. Order.

## 2. Get the display

1. Order a **Waveshare 7.5" e-Paper panel (800×480, monochrome)**.
2. Order a **Waveshare Universal e-Paper Driver HAT** separately.
3. Connect the panel's FPC cable into the Driver HAT's ZIF socket.
4. The Driver HAT's 8-pin output connects directly into J2 on the Athenaeum board.

## 3. Flash the firmware

See [`firmware/README.md`](../firmware/README.md) for toolchain setup and flashing instructions.

## 4. Assemble

1. Mount the assembled PCB somewhere secure — remember, no enclosure by design, so handle it like any exposed board.
2. Connect the Driver HAT/panel assembly to J2.
3. Connect a single-cell LiPo battery to the JST-PH connector (BT1) — mind polarity.
4. Insert a microSD card into J3.
5. Power on via USB-C.

## Known constraints / design notes

- The ESP32-S3 module's onboard antenna keep-out zone is kept clear of copper and components on the PCB — do not modify layout in that region without preserving the clearance.
- SPI bus (SCK/MOSI) is shared between the display and microSD card, each with an independent chip-select line — this is intentional, not a routing shortcut.
- The two mirrored USB-C D+/D- pins (for cable reversibility) are intentionally left unconnected on one side per pair — this is correct per the USB-C spec, not a missing connection.
