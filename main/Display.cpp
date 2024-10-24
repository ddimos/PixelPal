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

void Display::addBitmap(int16_t _x, int16_t _y, const uint8_t* _bitmap, int _width, int _size)
{
    ssd1306_vertical_bitmaps(&g_display, _x, _y, _bitmap, _width, _size, true);
}

void Display::getBuffer(uint8_t* _buffer)
{
    ssd1306_get_buffer(&g_display, _buffer);
}

void Display::setBuffer(const uint8_t* _buffer)
{
    ssd1306_set_buffer(&g_display, _buffer);
}

void Display::image(uint8_t* _buffer, uint8_t _w)
{
    ssd1306_display_image(&g_display, 1, 10, _buffer, _w);
}

void Display::display()
{
    ssd1306_show_buffer(&g_display);
}

void Display::clear()
{
    ssd1306_clear_screen(&g_display, false);
}

} // namespace pp
