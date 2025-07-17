#include <stdio.h>
#include "esp_timer.h"

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

  setCpuFrequencyMhz(240);  // Max out CPU speed
  Serial.printf("CPU Freq: %d MHz\n", getCpuFrequencyMhz());

  rom_init(gb_rom);     // Load built-in ROM
  sdl_init();           // Setup display and input
  gbc_mem_init();       // Game Boy memory setup
  cpu_init();           // Initialize CPU
}

void loop() {
  const int cycles_per_frame = 69905;
  int cycles = 0;

  int64_t frame_start = esp_timer_get_time();
  int64_t cpu_start = esp_timer_get_time();

  // Run emulator for one frame
  while (cycles < cycles_per_frame) {
    cycles += cpu_cycle();
    lcd_cycle();
    timer_cycle();
  }

  int64_t cpu_end = esp_timer_get_time();

  sdl_update();   // Read inputs
  sdl_frame();    // Draw frame to screen

  int64_t render_end = esp_timer_get_time();

  int64_t emu_time = cpu_end - frame_start;
  int64_t cpu_time = cpu_end - cpu_start;
  int64_t render_time = render_end - cpu_end;
  int64_t total_time = render_end - frame_start;

  Serial.printf("CPU: %lld us | Emu: %lld us | Render: %lld us | Total: %lld us | FPS: %.2f\n",
                cpu_time, emu_time, render_time, total_time,
                1000000.0 / total_time);

  // Delay to target ~60 FPS (optional)
  const int64_t frame_duration_us = 1000000 / 59.7;
  int64_t frame_elapsed = esp_timer_get_time() - frame_start;
  if (frame_elapsed < frame_duration_us) {
    delayMicroseconds(frame_duration_us - frame_elapsed);
  }
}
