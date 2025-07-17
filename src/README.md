# Game Boy Emulator Port for ESP32-S3 + ST7789 Display

---

This personal learning project ports a Game Boy emulator originally made for the ESP32-WROVER and ILI9341 display, adapting it to the ESP32-S3 with an ST7789 using the TFT_eSPI library. The emulator works but runs very slowly (~2–4 FPS).

---

## What This Is

- Emulator port for ESP32-S3 with ST7789 (240x240 or 240x320)  
- Uses TFT_eSPI instead of Adafruit GFX  
- Supports physical button input via GPIO  
- Runs at low performance, currently not playable  

---


## Why I Made This

I was originally looking to add a Game Boy emulator to my [esp_handheld project](https://github.com/ParkerFung/esp_handheld.git), but couldn’t find one compatible with my specific hardware setup. This led me to:

- Adapt existing emulator code from using one display library to another (Adafruit ST7789 → TFT_eSPI)  
- Profile and debug performance on an embedded system like the ESP32-S3  
- Understand the optimizations needed to run emulators efficiently on microcontrollers  

---

## Hardware Used

| Component         | Description                       |
|------------------|-----------------------------------|
| ESP32-S3 Board   | ESP32-S3-WROOM-1 dev board        |
| Display          | ST7789 240x240 SPI TFT            |
| Buttons          | 8 push buttons (wired to GPIO)    |

---

### Button Pin Connections

| Button       | GPIO Pin |
|--------------|----------|
| Up           | 10       |
| Down         | 21       |
| Left         | 11       |
| Right        | 12       |
| A            | 20       |
| B            | 7        |
| Start        | 9        |
| Select       | 15       |

> All buttons are active low — connect GPIO to GND when pressed.

---

### Display Pin Connections

| TFT Pin     | ESP32 GPIO Pin |
|-------------|----------------|
| TFT_CS      | 5              |
| TFT_DC      | 2              |
| TFT_MOSI    | 17             |
| TFT_SCLK    | 18             |
| TFT_BL      | 15 (backlight) |

No reset or MISO pins are used. Hardware SPI is required.  
Make sure these match your `User_Setup.h` or `platformio.ini` config for **TFT_eSPI**.

---

## What Works

- Emulator boots and displays output  
- Physical buttons register correctly  
- Display refresh works consistently  
- Framebuffer rendering (2-bit grayscale)  
- LCD and memory access are stable  
- ROMs can be loaded and begin execution  

---

## What Doesn’t Work Well

- **Main issue:** extremely low frame rate (~2–4 FPS)  
- Games technically run but are **not playable**  
- CPU cycle emulation is very slow  
- Not optimized for ESP32-S3 yet  

---

## Performance Debugging (In Progress)

I did some basic profiling to figure out where time is being spent.

### Key Findings:
- Frame rendering only takes **~9700 µs**  
- CPU emulation takes **~260,000 to 530,000 µs**  
- FPS measured between **1.8 and 3.7**  
- Memory, input, and display logic are *not* the bottleneck  
- Main loop and frame pacing logic are functioning correctly

### Debug Tools Added:
- Timing using `esp_timer_get_time()`  
- Per-opcode timing counters inside `cpu_cycle()`  
- Serial debug output of frame time and FPS  
- External arrays to track slow instructions  
- Optional frame delay to target 59.7 FPS (for future optimization)

---

### Sample Debug Output

CPU: 261467 us | Emu: 261467 us | Render: 9689 us | Total: 271156 us | FPS: 3.69
CPU: 417968 us | Emu: 417968 us | Render: 9698 us | Total: 427666 us | FPS: 2.34
CPU: 530069 us | Emu: 530069 us | Render: 9678 us | Total: 539747 us | FPS: 1.85



This clearly shows the majority of frame time is spent on CPU emulation, not rendering.

---

## Code Changes Summary

- Replaced Adafruit ST7789 + GFX libraries with **TFT_eSPI** for faster display handling  
- Adapted pin mappings and removed reset/MISO pins unused by ST7789  
- Updated button GPIO assignments for my custom wiring  
- Added timing instrumentation inside `loop()` and `cpu_cycle()` to profile performance  
- Defined and exported opcode timing counters to identify slow instructions  

---

## Next Steps and Future Plans

- Output top slowest opcodes to identify optimization targets  
- Optimize opcode handlers and memory access routines  
- Use `IRAM_ATTR` to place hot functions in instruction RAM  
- Explore alternate rendering techniques or hardware acceleration if available  

---

## Hardware Requirements

- ESP32-S3 microcontroller (tested on ESP32-S3-WROOM-1)  
- ST7789 TFT display (240x240 or 240x320 SPI interface)  
- Physical buttons wired between GPIO and ground (active low)  
- USB or battery power source  

---

## How To Build and Flash

1. Install [PlatformIO](https://platformio.org/) or use Arduino IDE with ESP32 support  
2. Clone this repository  
3. Install **TFT_eSPI** library  
4. Configure pin assignments in `User_Setup.h` or `platformio.ini` to match your hardware  
5. Compile and upload the code to your ESP32-S3  
6. Connect buttons and display per the pinout above  
7. Power on and observe the emulator boot screen  

---

## Contributing

Contributions are very welcome! If you'd like to help optimize, fix bugs, or add features, please feel free to open issues or submit pull requests.

Before submitting changes, please ensure:
- Your code is well-documented and clean.
- Changes are focused and clearly explained in the PR description.
- You test your changes on actual hardware if possible.

Thanks for your interest and support!

---


## License and Credits

This project is based on the original work from:

https://github.com/lualiliu/esp32-gameboy.git  
(C) lualiliu and contributors


Special thanks to:  
- lualiliu for the original ESP32 Game Boy emulator  
- Bodmer for the TFT_eSPI library  
- The ESP32 and Arduino development communities  

---

## Final Notes

This emulator port is a work-in-progress and primarily a learning tool for me to explore:

- Embedded emulator development  
- Performance profiling on microcontrollers  
- Display driver adaptation  
- ESP32-S3 hardware capabilities  

Though it’s not yet performant enough for smooth gameplay, I’m excited to keep improving it and hope others can learn from my experience.

---

**Created by Parker Fung**  
*July 2025*  
