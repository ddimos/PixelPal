#pragma once
#include "esp_timer.h"

namespace pp
{

class PeriodicTimer
{
public:
    PeriodicTimer(uint64_t _periodUs, esp_timer_cb_t _callback);
    ~PeriodicTimer();
private:
    esp_timer_handle_t m_timerHandle = NULL;
};

} // namespace pp
