#pragma once
#include <array>
#include <functional>
#include <driver/gpio.h>

namespace pp::input
{

struct Button
{
    gpio_num_t gpio = gpio_num_t::GPIO_NUM_NC;
    bool isPressed = false;
};

class InputManager
{
public:
    static constexpr unsigned NumberOfSupportedButtons = 4; // TODO add to the config

    using ButtonsArray = std::array<Button, NumberOfSupportedButtons>;
    using OnButtonPressedCallback = std::function<void(const Button&)>;
    using OnButtonReleasedCallback = std::function<void(const Button&)>;
    // Note: using std::function takes an additional ~1kB of flash memory, but it allows to use captures.

    InputManager() = default;
    ~InputManager() = default;

    void init(ButtonsArray _buttons, OnButtonPressedCallback _pressedCb, OnButtonReleasedCallback _releaseCb);
    void update();

private:
    ButtonsArray m_buttons;
    OnButtonPressedCallback m_pressedCallback;
    OnButtonReleasedCallback m_releaseCallback;
};

} // namespace pp::input
