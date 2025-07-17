/*
================================================================================
Game Boy Emulator on ESP32 with ST7789 TFT Display
================================================================================

Overview:
---------
This project is a hardware adaptation and port of the Game Boy emulator from:

https://github.com/lualiliu/esp32-gameboy.git

The emulator runs on an ESP32 microcontroller with an ST7789 TFT display,
rendering classic Game Boy games in real time. Input is handled via physical
buttons wired to specific GPIO pins.

Modifications:
--------------
- Switched from Adafruit ST7789 + GFX libraries to the faster TFT_eSPI library
- Updated GPIO pin assignments to fit custom hardware wiring
- Adjusted TFT SPI pin configuration (no reset or MISO pins used)
- Minor code cleanups for compatibility with ESP32-S3 and display variants

Features:
---------
- Game Boy emulator supporting multiple ROM mappers
- ST7789 TFT display (240x240 or 240x320) via hardware SPI
- Physical buttons for Up, Down, Left, Right, A, B, Start, Select (active low)
- TFT backlight control via dedicated GPIO
- Efficient 2-bit grayscale framebuffer rendering
- DMA handling for OAM transfers
- Modular design with CPU, memory, LCD, timer, interrupt, input, and display modules

Hardware Requirements:
----------------------
- ESP32 microcontroller (ESP32-S3 or compatible)
- ST7789 TFT display (240x240 or 240x320)
- Physical push buttons wired to ESP32 GPIOs and connected to ground when pressed

Pin Configuration:
------------------
| Function             | GPIO Pin |
|----------------------|----------|
| TFT CS               | 5        |
| TFT DC               | 2        |
| TFT MOSI             | 17       |
| TFT SCLK             | 18       |
| TFT Backlight (LED)  | 15       |
| Button Up            | 10       |
| Button Down          | 21       |
| Button Left          | 11       |
| Button Right         | 12       |
| Button A             | 20       |
| Button B             | 7        |
| Button Start         | 9        |
| Button Select        | 15       |

Note: All buttons are active low (connected to ground when pressed).

Software Setup:
---------------
Libraries Required:
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) Library (used instead of Adafruit ST7789)
- Arduino core for ESP32 with SPI support

> ⚠️ Configure your `User_Setup.h` or `platformio.ini` to match your hardware pinout for TFT_eSPI.

Building and Flashing:
----------------------
1. Set up the ESP32 development environment (Arduino IDE or PlatformIO)
2. Install the required libraries (see above)
3. Connect hardware per the pinout above
4. Compile and upload code to the ESP32
5. Power on device; TFT backlight should activate and emulator screen appear

Usage:
------
- Use physical buttons to control the Game Boy emulator
- The display renders frames in real time
- Backlight controlled via GPIO 15

Troubleshooting:
----------------
- Blank screen with backlight: verify SPI wiring and pin assignments
- No button response: ensure buttons are connected to ground and GPIO pins are correct
- Flickering or artifacts: try lowering SPI clock speed, check power supply stability

License and Credits:
--------------------
This project is a derivative work based on:

https://github.com/lualiliu/esp32-gameboy.git  
Copyright (c) lualiliu and contributors

Original project is licensed under GNU General Public License v3.0 (GPLv3).

This adaptation inherits the same GPLv3 license. See LICENSE file for details.

Acknowledgments:
----------------
- Original author lualiliu for the ESP32 Game Boy emulator
- Bodmer for the TFT_eSPI library
- ESP32 and Arduino development communities

================================================================================
*/
