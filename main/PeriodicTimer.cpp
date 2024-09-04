#include "PeriodicTimer.hpp"

#include "esp_log.h"
#include "esp_timer.h"

namespace
{
static const char* TAG = "PP_TIMER";
} // namespace


namespace pp
{

PeriodicTimer::PeriodicTimer(uint64_t _periodUs, esp_timer_cb_t _callback)
{
    const esp_timer_create_args_t periodicTimerArgs = {
        .callback = _callback,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "periodic",
        .skip_unhandled_events = false
    };

    ESP_ERROR_CHECK(esp_timer_create(&periodicTimerArgs, &m_timerHandle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(m_timerHandle, _periodUs));
    ESP_LOGI(TAG, "Started timer, time since boot: %lld us", esp_timer_get_time());
}

PeriodicTimer::~PeriodicTimer()
{
    ESP_ERROR_CHECK(esp_timer_stop(m_timerHandle));
    ESP_ERROR_CHECK(esp_timer_delete(m_timerHandle));
    ESP_LOGI(TAG, "Stop timer, time since boot: %lld us", esp_timer_get_time());
}

} // namespace pp