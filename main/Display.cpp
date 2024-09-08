#include "Display.hpp"

#include "esp_log.h"

extern "C"
{
#include "ssd1306.h"
}

namespace
{
static SSD1306_t g_display;
static const char* TAG = "PP_DISPLAY";
} // namespace

namespace pp
{


void Display::init()
{
    i2c_master_init(&g_display, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);

    ESP_LOGI(TAG, "Init display");
    ssd1306_init(&g_display, 128, 64);

    ssd1306_clear_screen(&g_display, false);
    ssd1306_contrast(&g_display, 0xff);
}

void Display::deinit()
{
}

void Display::drawText(const char* _text, int _length, int _pos)
{
    ssd1306_display_text(&g_display, _pos, _text, _length, false);
}

void Display::drawBitmap(int _x, int _y, uint8_t* _bitmap, int _width, int _height)
{
    ssd1306_bitmaps(&g_display, _x, _y, _bitmap, _width, _height, false);
}

void Display::clear()
{
    ssd1306_clear_screen(&g_display, false);
}

} // namespace pp
