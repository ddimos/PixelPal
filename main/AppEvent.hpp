#pragma once
#include "Types.hpp"

namespace pp
{
struct AppEvent
{
    enum class Type : uint8_t
    {
        Tick,
        ButtonPressed,
        ButtonReleased
    };
    ButtonName button = ButtonName::None;
    Type type;
};
} // namespace pp
