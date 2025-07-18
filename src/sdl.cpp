#include <TFT_eSPI.h>
#include "driver/gpio.h"

#define GAMEBOY_WIDTH 160
#define GAMEBOY_HEIGHT 144
#define TFT_LED_PIN 15

// Scale Game Boy screen 1.5x to fit on 320x240 display
#define SCALED_WIDTH 240   
#define SCALED_HEIGHT 216  

// Center the scaled image on the screen
#define OFFSET_X ((320 - SCALED_WIDTH) / 2)
#define OFFSET_Y ((240 - SCALED_HEIGHT) / 2)

// Framebuffer: 2-bit grayscale, 4 pixels per byte
byte pixels[GAMEBOY_WIDTH * GAMEBOY_HEIGHT / 4];

// Map 2-bit grayscale pixels to RGB565 colors
const uint16_t color[] = { TFT_WHITE, 0xAAAA, 0x5555, TFT_BLACK };

static TFT_eSPI tft = TFT_eSPI();

static int button_start, button_select, button_a, button_b;
static int button_down, button_up, button_left, button_right;

// Control backlight on/off
void backlighting(bool state) {
    digitalWrite(TFT_LED_PIN, state ? HIGH : LOW);
}

// Extract 2-bit pixel color index from framebuffer
byte getColorIndexFromFrameBuffer(int x, int y) {
    int offset = x + y * GAMEBOY_WIDTH;
    return (pixels[offset >> 2] >> ((offset & 3) << 1)) & 3;
}

// Scale and draw the framebuffer to the display centered
void SDL_Flip(byte *screen) {
    static uint16_t pixel_buffer[SCALED_WIDTH * SCALED_HEIGHT];
    int idx = 0;

    const int32_t ratio_x = (GAMEBOY_WIDTH << 16) / SCALED_WIDTH;
    const int32_t ratio_y = (GAMEBOY_HEIGHT << 16) / SCALED_HEIGHT;

    int32_t src_y = 0;

    for (int y = 0; y < SCALED_HEIGHT; y++) {
        int gb_y = src_y >> 16;
        src_y += ratio_y;

        int32_t src_x = 0;
        for (int x = 0; x < SCALED_WIDTH; x++) {
            int gb_x = src_x >> 16;
            src_x += ratio_x;

            pixel_buffer[idx++] = color[getColorIndexFromFrameBuffer(gb_x, gb_y)];
        }
    }

    tft.pushImage(OFFSET_X, OFFSET_Y, SCALED_WIDTH, SCALED_HEIGHT, pixel_buffer);
}

// Initialize display, backlight, and configure buttons
void sdl_init(void) {
    pinMode(TFT_LED_PIN, OUTPUT);
    backlighting(true);

    tft.init();
    tft.setRotation(1);  // Landscape orientation
    tft.fillScreen(TFT_BLACK);

    gpio_num_t button_pins[] = {
        GPIO_NUM_10, GPIO_NUM_21, GPIO_NUM_11, GPIO_NUM_12,
        GPIO_NUM_15, GPIO_NUM_7,  GPIO_NUM_20, GPIO_NUM_9
    };

    gpio_config_t io_conf = {};
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    for (int i = 0; i < sizeof(button_pins) / sizeof(button_pins[0]); ++i) {
        io_conf.pin_bit_mask = 1ULL << button_pins[i];
        gpio_config(&io_conf);
    }
}

// Read button states (active low)
int sdl_update(void) {
    button_up     = !gpio_get_level(GPIO_NUM_10);
    button_down   = !gpio_get_level(GPIO_NUM_21);
    button_left   = !gpio_get_level(GPIO_NUM_11);
    button_right  = !gpio_get_level(GPIO_NUM_12);
    button_select = !gpio_get_level(GPIO_NUM_15);
    button_b      = !gpio_get_level(GPIO_NUM_7);
    button_a      = !gpio_get_level(GPIO_NUM_20);
    button_start  = !gpio_get_level(GPIO_NUM_9);
    return 0;
}

// Return button bitfield for emulator input
unsigned int sdl_get_buttons(void) {
    return (button_start << 3) | (button_select << 2) | (button_b << 1) | button_a;
}

// Return direction bitfield for emulator input
unsigned int sdl_get_directions(void) {
    return (button_down << 3) | (button_up << 2) | (button_left << 1) | button_right;
}

// Return pointer to framebuffer pixels
byte* sdl_get_framebuffer(void) {
    return pixels;
}

// Render the current frame to the display
void sdl_frame(void) {
    SDL_Flip(pixels);
}
