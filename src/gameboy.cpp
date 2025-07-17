// #include <stdio.h>
// #include "timer.h"
// #include "rom.h"
// #include "mem.h"
// #include "cpu.h"
// #include "lcd.h"
// #include "sdl.h"
// #include "gbrom.h"


// void setup() {
//   // put your setup code here, to run once:
//   int r = rom_init(gb_rom);

//   sdl_init();

//   printf("ROM OK!\n");

//   gbc_mem_init();
//   printf("Mem OK!\n");

//   cpu_init();
//   printf("CPU OK!\n");
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//     cpu_cycle();

//     lcd_cycle();

//     timer_cycle();
// }

// #include <stdio.h>
// #include "timer.h"
// #include "rom.h"
// #include "mem.h"
// #include "cpu.h"
// #include "lcd.h"
// #include "sdl.h"
// #include "gbrom.h"

// // The Arduino framework uses setup() and loop(), so we define them:
// void setup() {
//   rom_init(gb_rom);      // Load ROM into memory
//   sdl_init();            // Initialize display and GPIO
//   gbc_mem_init();        // Initialize Game Boy memory
//   cpu_init();            // Initialize CPU
//   printf("Ready!\n");
// }

// void loop() {
//   const int cycles_per_frame = 69905;
//   int cycles = 0;

//   // Run enough cycles for one frame (Game Boy = ~59.7 FPS)
//   while (cycles < cycles_per_frame) {
//     cycles += cpu_cycle();

//     // Synchronize peripherals
//     lcd_cycle();
//     timer_cycle();
//   }

//   sdl_update();  // Read button input
//   sdl_frame();   // Render one frame to display

//   // Optional: stabilize SPI timing (not required, but helps)
//   delay(1);
// }
//------------------------------------------------------------------------
#include <stdio.h>
#include "esp_timer.h"     // For precise timing
#include "timer.h"
#include "rom.h"
#include "mem.h"
#include "cpu.h"
#include "lcd.h"
#include "sdl.h"
#include "gbrom.h"

void setup() {
  Serial.begin(115200);
  printf("Ready!\n");

  setCpuFrequencyMhz(240);  // Set to 240MHz
  Serial.printf("CPU Freq: %d MHz\n", getCpuFrequencyMhz());
  
  rom_init(gb_rom);      // Load ROM into memory
  sdl_init();            // Initialize display and GPIO
  gbc_mem_init();        // Initialize Game Boy memory
  cpu_init();            // Initialize CPU
}

// void loop() {
//   const int cycles_per_frame = 69905;
//   int cycles = 0;

//   int64_t frame_start = esp_timer_get_time();

//   // Run CPU and peripherals for one full frame
//   while (cycles < cycles_per_frame) {
//     cycles += cpu_cycle();
//     lcd_cycle();
//     timer_cycle();
//   }

//   int64_t emu_end = esp_timer_get_time();

//   sdl_update();  // Read inputs
//   sdl_frame();   // Draw framebuffer to screen

//   int64_t render_end = esp_timer_get_time();

//   int64_t emu_time = emu_end - frame_start;
//   int64_t render_time = render_end - emu_end;
//   int64_t total_time = render_end - frame_start;

//   Serial.printf("Emu: %lld us | Render: %lld us | Total: %lld us | FPS: %.2f\n",
//                 emu_time, render_time, total_time,
//                 1000000.0 / total_time);

//   // Frame delay to cap at ~59.7 FPS (optional)
//   const int64_t frame_duration_us = 1000000 / 59.7;
//   int64_t frame_elapsed = esp_timer_get_time() - frame_start;
//   if (frame_elapsed < frame_duration_us) {
//     delayMicroseconds(frame_duration_us - frame_elapsed);
//   }
// }
void loop() {
  const int cycles_per_frame = 69905;
  int cycles = 0;

  int64_t frame_start = esp_timer_get_time();

  int64_t cpu_start = esp_timer_get_time();
  
  // Run CPU and peripheral cycles for one full Game Boy frame
  while (cycles < cycles_per_frame) {
    cycles += cpu_cycle();
    lcd_cycle();
    timer_cycle();
  }

  int64_t cpu_end = esp_timer_get_time();

  sdl_update();  // Read inputs
  sdl_frame();   // Draw framebuffer to screen

  int64_t render_end = esp_timer_get_time();

  int64_t emu_time = cpu_end - frame_start;
  int64_t cpu_time = cpu_end - cpu_start;
  int64_t render_time = render_end - cpu_end;
  int64_t total_time = render_end - frame_start;

  Serial.printf("CPU: %lld us | Emu: %lld us | Render: %lld us | Total: %lld us | FPS: %.2f\n",
                cpu_time, emu_time, render_time, total_time,
                1000000.0 / total_time);

  // Optional: frame delay to target ~59.7 FPS
  const int64_t frame_duration_us = 1000000 / 59.7;
  int64_t frame_elapsed = esp_timer_get_time() - frame_start;
  if (frame_elapsed < frame_duration_us) {
    delayMicroseconds(frame_duration_us - frame_elapsed);
  }
}
