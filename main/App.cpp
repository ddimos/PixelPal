#include "App.hpp"
#include "PeriodicTimer.hpp"
#include "InputManager.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#include "esp_log.h"
#include "esp_timer.h"

namespace
{
static const char* TAG = "PP_APP";
} // namespace

namespace pp
{

void App::start()
{
    //TODO CONFIG_ESP32_BUTTON_QUEUE_SIZE
    QueueHandle_t queueHandle = xQueueCreate(5, sizeof(AppEvent));
    m_queueHandle = queueHandle;

    m_display.init();
    m_inputManager.init(
        {
            input::Button{ .gpio = gpio_num_t::GPIO_NUM_25 },
            input::Button{ .gpio = gpio_num_t::GPIO_NUM_26 },
            input::Button{ .gpio = gpio_num_t::GPIO_NUM_27 },
            input::Button{ .gpio = gpio_num_t::GPIO_NUM_14 }
        },
        [queueHandle](const pp::input::Button& _button){
            pp::AppEvent event{
                .pin = (uint8_t)_button.gpio,
                .event = pp::AppEvent::Type::ButtonPressed,
            };
            xQueueSend(queueHandle, &event, portMAX_DELAY);
        },
        [queueHandle](const pp::input::Button& _button){
            pp::AppEvent event{
                .pin = (uint8_t)_button.gpio,
                .event = pp::AppEvent::Type::ButtonReleased,
            };
            xQueueSend(queueHandle, &event, portMAX_DELAY);
        }
    );

    // TODO CONFIG_ESP32_BUTTON_TASK_STACK_SIZE
    xTaskCreate(&buttonTask, "buttonTask", CONFIG_ESP_MAIN_TASK_STACK_SIZE, &m_inputManager, 10, NULL);    // I could make the stack size smaller
    xTaskCreate(&mainTask, "mainTask", CONFIG_ESP_MAIN_TASK_STACK_SIZE, this, 9, NULL);

    m_state = State::Init;
    dispatchToState(m_state, StateEvent::STATE_ENTER);

    // We will return from app_main after initializing everything
}

void App::mainTask(void* _pvParameter)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(40); // // TODO add to config

    App* app = static_cast<App*>(_pvParameter);

    while (true)
    {
        AppEvent event;
        while( xQueueReceive( app->m_queueHandle, &event, 0 ))
        {
            ESP_LOGI(TAG, "handling  %d %d, ", (int)event.event, event.pin);
            if (event.event != AppEvent::Type::ButtonPressed)
                continue;
            
            StateEvent stateEvent = app->appToStateEvent(event);
            
            app->dispatch(stateEvent);
        }

        app->dispatch(StateEvent::TICK);
        
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}

void App::buttonTask(void* _pvParameter)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(20); // // TODO add to config

    pp::input::InputManager* inputManager = static_cast<pp::input::InputManager*>(_pvParameter);

    while (true)
    {
        inputManager->update();
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}  

void App::dispatch(StateEvent _event)
{
    State prevState = m_state;
    StateStatus status = dispatchToState(m_state, _event);
    if (status == StateStatus::Transit)
    {
        dispatchToState(prevState, StateEvent::STATE_EXIT);
        dispatchToState(m_state, StateEvent::STATE_ENTER);
    }
}

App::StateEvent App::appToStateEvent(AppEvent _event) const
{
    StateEvent stateEvent = StateEvent::None;
    switch (_event.pin)
    {
        case gpio_num_t::GPIO_NUM_25:
            stateEvent = pp::App::StateEvent::BUTTON_RIGHT;
            break;
        case gpio_num_t::GPIO_NUM_26:
            stateEvent = pp::App::StateEvent::BUTTON_UP;
            break;
        case gpio_num_t::GPIO_NUM_27:
            stateEvent = pp::App::StateEvent::BUTTON_LEFT;
            break;
        case gpio_num_t::GPIO_NUM_14:
            stateEvent = pp::App::StateEvent::BUTTON_DOWN;
            break;
        default:
            break;
        
    }
    return stateEvent;
}

App::StateStatus App::dispatchToState(State _state, StateEvent _event)
{
    StateStatus status = StateStatus::Ignored;
    switch (_state)
    {
    case State::Init:
        status = dispatchInitState(_event);
        break;
    case State::Menu:
        status = dispatchMenuState(_event);
        break;
    case State::Settings:
        status = dispatchSettingsState(_event);
        break;
    case State::Game:
        status = dispatchGameState(_event);
        break;
    case State::Finish:
        status = dispatchFinishState(_event);
        break;    
    default:
        break;
    }
    return status;
}

App::StateStatus App::dispatchInitState(StateEvent _event)
{
    StateStatus status = StateStatus::Ignored;
    switch (_event)
    {
    case StateEvent::STATE_ENTER:
        m_transitFromInitAfterCount = 20;
        m_display.drawText("    PixelPal", 12, 4);
        break;
    case StateEvent::STATE_EXIT:
        m_display.clear();
        break;
    case StateEvent::TICK:
        --m_transitFromInitAfterCount;
        if (m_transitFromInitAfterCount <= 0)
        {
            status = StateStatus::Transit;
            m_state = State::Menu;
        }
        break;
    default:
        break;
    }
    return status;
}

App::StateStatus App::dispatchMenuState(StateEvent _event)
{
    StateStatus status = StateStatus::Ignored;

    switch (_event)
    {
    case StateEvent::STATE_ENTER:
        m_display.drawText("Start     <", 11, 3);
        m_display.drawText("Settings   ", 11, 5);
        m_cursorPos = 0;
        break;
    case StateEvent::STATE_EXIT:
        m_display.clear();
        break;
    case StateEvent::BUTTON_DOWN:
        if (m_cursorPos == 0)
        {
            m_cursorPos++;
            m_display.drawText("Start      ", 11, 3);
            m_display.drawText("Settings  <", 11, 5);
        }
        break;
    case StateEvent::BUTTON_UP:
        if (m_cursorPos == 1)
        {
            m_cursorPos--;
            m_display.drawText("Start     <", 11, 3);
            m_display.drawText("Settings   ", 11, 5);
        }
        break;
    case StateEvent::BUTTON_RIGHT:
        status = StateStatus::Transit;
        m_state = m_cursorPos == 0 ? State::Game : State::Settings;
        break;
    default:
        break;
    }
    return status;
}

App::StateStatus App::dispatchSettingsState(StateEvent _event)
{
    StateStatus status = StateStatus::Ignored;

    switch (_event)
    {
    case StateEvent::STATE_ENTER:
        m_display.drawText("   Settings", 11, 2);
        break;
    case StateEvent::STATE_EXIT:
        m_display.clear();
        break;
    case StateEvent::BUTTON_LEFT:
        status = StateStatus::Transit;
        m_state = State::Menu;
        break;
    default:
        break;
    }
    return status;
}

App::StateStatus App::dispatchGameState(StateEvent _event)
{
    StateStatus status = StateStatus::Ignored;
    switch (_event)
    {
    case StateEvent::STATE_ENTER:
        m_display.drawText("   PLAY", 7, 2);
        break;
    case StateEvent::STATE_EXIT:
        break;
    default:
        break;
    }
    return status;
}

App::StateStatus App::dispatchFinishState(StateEvent _event)
{
    StateStatus status = StateStatus::Ignored;
    switch (_event)
    {
    case StateEvent::STATE_ENTER:
        break;
    case StateEvent::STATE_EXIT:
        break;
    default:
        break;
    }
    return status;
}

} // namespace pp
