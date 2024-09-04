#include "InputManager.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/gpio.h>

#include "esp_log.h"

namespace
{
static const char* TAG = "PP_INPUT";
}

namespace pp::input
{

void InputManager::init(ButtonsArray _buttons, OnButtonPressedCallback _pressedCb, OnButtonReleasedCallback _releaseCb)
{
    m_buttons = _buttons;
    m_pressedCallback = _pressedCb;
    m_releaseCallback = _releaseCb;

    for(const Button& btn : m_buttons)
    {
        ESP_LOGD(TAG, "Init button, gpio %d", btn.gpio);
        gpio_reset_pin(btn.gpio);
        gpio_set_direction(btn.gpio, GPIO_MODE_INPUT);
        gpio_pullup_en(btn.gpio);
    }
}

void InputManager::update()
{
    // TODO: implement a debouncing algorithm 
    for(Button& btn : m_buttons)
    {
        if (gpio_get_level(btn.gpio) == 0)
        {
            if (!btn.isPressed)
            {
                m_pressedCallback(btn);
                btn.isPressed = true;
            }
        }
        else
        {
            if (btn.isPressed)
            {
                m_releaseCallback(btn);
                btn.isPressed = false;
            }
        }
    }
}

} // namespace pp::input
