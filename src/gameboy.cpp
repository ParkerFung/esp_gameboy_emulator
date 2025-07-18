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

const int cycles_per_frame = 69905;
const int frame_skip = 4;   // Render every 4 frames
int frame_count = 0;

void loop() {
  int cycles = 0;
  int64_t frame_start = esp_timer_get_time();
  int64_t cpu_start = esp_timer_get_time();

  // Run CPU emulation for one frame worth of cycles
  while (cycles < cycles_per_frame) {
    cycles += cpu_cycle();
    lcd_cycle();
    timer_cycle();
  }

  int64_t cpu_end = esp_timer_get_time();

  sdl_update();  // Always update input

  // Only render on selected frames to save time
  if (frame_count % frame_skip == 0) {
    sdl_frame();
  }

  frame_count++;

  int64_t render_end = esp_timer_get_time();
  int64_t total_time = render_end - frame_start;

  // Optional: delay to maintain ~60 FPS if we're running too fast
  const int64_t frame_duration_us = 1000000 / 59.7;
  int64_t frame_elapsed = esp_timer_get_time() - frame_start;
  if (frame_elapsed < frame_duration_us) {
    delayMicroseconds(frame_duration_us - frame_elapsed);
  }
}