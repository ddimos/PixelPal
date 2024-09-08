#pragma once
#include "sdkconfig.h"

namespace pp
{
float constexpr TICK_DT_S = CONFIG_PP_TICK_DT_MS / 1000.f;
} // namespace pp
