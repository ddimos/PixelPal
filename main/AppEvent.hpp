#pragma once

#include <stdint.h>

namespace pp
{
struct AppEvent
{
    enum class Type : uint8_t
    {
        ButtonPressed,
        ButtonReleased
    };
    uint8_t pin;
    Type event;
};
} // namespace pp
