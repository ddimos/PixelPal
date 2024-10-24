#pragma once
#include "sdkconfig.h"

namespace pp
{
constexpr float TICK_DT_S = CONFIG_PP_TICK_DT_MS / 1000.f;

constexpr unsigned MAP_WIDTH = 128;
constexpr unsigned MAP_HEIGHT = 64;

} // namespace pp
