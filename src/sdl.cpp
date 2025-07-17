// #include "SPI.h"
// #include "Adafruit_GFX.h"
// #include <Adafruit_ST7789.h>
// #include "driver/gpio.h"

// #define _cs   5   // TFT CS pin
// #define _dc   2   // TFT DC pin
// #define _mosi 17   // TFT MOSI pin
// #define _sclk 18   // TFT SCLK pin
// #define _rst  4   // TFT RESET pin
// // #define _miso 25   // Not connected
// #define _led   15   // TFT LED pin (backlight)

// Adafruit_ST7789 tft = Adafruit_ST7789(_cs, _dc, _mosi, _sclk, _rst);

// void backlighting(bool state) {
//     digitalWrite(_led, state ? HIGH : LOW);
// }

// #define GAMEBOY_HEIGHT 160
// #define GAMEBOY_WIDTH 144
// byte pixels[GAMEBOY_HEIGHT * GAMEBOY_WIDTH / 4];

// // Button states
// static int button_start, button_select, button_a, button_b, button_down, button_up, button_left, button_right;

// byte getColorIndexFromFrameBuffer(int x, int y) {
//     int offset = x + y * GAMEBOY_HEIGHT;
//     return (pixels[offset >> 2] >> ((offset & 3) << 1)) & 3;
// }

// const uint16_t color[] = {ST77XX_BLACK, 0x5555, 0xAAAA, ST77XX_WHITE};

// void SDL_Flip(byte *screen) {
//     for (int i = 0; i < GAMEBOY_WIDTH; i++) {
//         for (int j = 0; j < GAMEBOY_HEIGHT; j++) {
//             tft.drawPixel(j, i, color[getColorIndexFromFrameBuffer(j, i)]);
//         }
//     }
// }

// void sdl_init(void)
// {
//     pinMode(_led, OUTPUT);
//     backlighting(true);

//     tft.init(240, 320);
//     tft.setRotation(1);

//     tft.fillScreen(ST77XX_BLACK);

//     // Configure GPIO buttons with internal pull-ups
//     gpio_num_t button_pins[] = {
//         GPIO_NUM_10, // Up
//         GPIO_NUM_21, // Down
//         GPIO_NUM_11, // Left
//         GPIO_NUM_12, // Right
//         GPIO_NUM_15, // Select
//         GPIO_NUM_7,  // B
//         GPIO_NUM_20, // A
//         GPIO_NUM_9   // Start
//     };

//     gpio_config_t io_conf = {};
//     io_conf.mode = GPIO_MODE_INPUT;
//     io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
//     io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
//     io_conf.intr_type = GPIO_INTR_DISABLE;

//     for (int i = 0; i < sizeof(button_pins) / sizeof(button_pins[0]); ++i) {
//         io_conf.pin_bit_mask = 1ULL << button_pins[i];
//         gpio_config(&io_conf);
//     }
// }

// int sdl_update(void) {
//     button_up     = !gpio_get_level(GPIO_NUM_10);
//     button_down   = !gpio_get_level(GPIO_NUM_21);
//     button_left   = !gpio_get_level(GPIO_NUM_11);
//     button_right  = !gpio_get_level(GPIO_NUM_12);
//     button_select = !gpio_get_level(GPIO_NUM_15);
//     button_b      = !gpio_get_level(GPIO_NUM_7);
//     button_a      = !gpio_get_level(GPIO_NUM_20);
//     button_start  = !gpio_get_level(GPIO_NUM_9);
//     return 0;
// }

// unsigned int sdl_get_buttons(void)
// {
//     return (button_start * 8) | (button_select * 4) | (button_b * 2) | button_a;
// }

// unsigned int sdl_get_directions(void)
// {
//     return (button_down * 8) | (button_up * 4) | (button_left * 2) | button_right;
// }

// byte* sdl_get_framebuffer(void)
// {
//     return pixels;
// }

// void sdl_frame(void)
// {
//     SDL_Flip(pixels);
// }


#include <TFT_eSPI.h>
#include "driver/gpio.h"

#define GAMEBOY_WIDTH 160
#define GAMEBOY_HEIGHT 144
#define TFT_LED_PIN 15

// Framebuffer storing 2-bit pixels packed 4 pixels per byte
byte pixels[GAMEBOY_WIDTH * GAMEBOY_HEIGHT / 4];

// Colors for 4 grayscale levels (RGB565)
const uint16_t color[] = { TFT_WHITE, 0xAAAA, 0x5555, TFT_BLACK };


static TFT_eSPI tft = TFT_eSPI();

// Button states
static int button_start, button_select, button_a, button_b;
static int button_down, button_up, button_left, button_right;

void backlighting(bool state) {
    digitalWrite(TFT_LED_PIN, state ? HIGH : LOW);
}

// Corrected indexing for framebuffer: width * y + x
byte getColorIndexFromFrameBuffer(int x, int y) {
    int offset = x + y * GAMEBOY_WIDTH;
    return (pixels[offset >> 2] >> ((offset & 3) << 1)) & 3;
}

// Optimized frame flip: build full buffer and push once
void SDL_Flip(byte *screen) {
    static uint16_t pixel_buffer[GAMEBOY_WIDTH * GAMEBOY_HEIGHT];
    int idx = 0;
    for (int y = 0; y < GAMEBOY_HEIGHT; y++) {
        for (int x = 0; x < GAMEBOY_WIDTH; x++) {
            pixel_buffer[idx++] = color[getColorIndexFromFrameBuffer(x, y)];
        }
    }
    tft.pushImage(0, 0, GAMEBOY_WIDTH, GAMEBOY_HEIGHT, pixel_buffer);
}

void sdl_init(void)
{
    pinMode(TFT_LED_PIN, OUTPUT);
    backlighting(true);

    tft.init();

    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    // Configure GPIO buttons with internal pull-ups
    gpio_num_t button_pins[] = {
        GPIO_NUM_10, // Up
        GPIO_NUM_21, // Down
        GPIO_NUM_11, // Left
        GPIO_NUM_12, // Right
        GPIO_NUM_15, // Select
        GPIO_NUM_7,  // B
        GPIO_NUM_20, // A
        GPIO_NUM_9   // Start
    };

    gpio_config_t io_conf = {};
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;

    for (int i = 0; i < sizeof(button_pins) / sizeof(button_pins[0]); ++i) {
        io_conf.pin_bit_mask = 1ULL << button_pins[i];
        gpio_config(&io_conf);
    }
}

int sdl_update(void) {
    // Update button states (active low)
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

unsigned int sdl_get_buttons(void)
{
    return (button_start * 8) | (button_select * 4) | (button_b * 2) | button_a;
}

unsigned int sdl_get_directions(void)
{
    return (button_down * 8) | (button_up * 4) | (button_left * 2) | button_right;
}

byte* sdl_get_framebuffer(void)
{
    return pixels;
}

void sdl_frame(void)
{
    SDL_Flip(pixels);
}

//----------------------------------------------------------------------------------------------------
