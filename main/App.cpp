#include "App.hpp"
#include "PeriodicTimer.hpp"
#include "InputManager.hpp"
#include "Types.hpp"
#include "utils/Assert.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#include "esp_log.h"
#include "esp_timer.h"

namespace
{
static const char* TAG = "PP_APP";

gpio_num_t buttonToGpio(pp::ButtonName _name)
{
    gpio_num_t gpio = gpio_num_t::GPIO_NUM_NC; 
    switch (_name)
    {
    case pp::ButtonName::RIGHT:
        gpio = gpio_num_t::GPIO_NUM_25;
        break;
    case pp::ButtonName::UP:
        gpio = gpio_num_t::GPIO_NUM_26;
        break;
    case pp::ButtonName::LEFT:
        gpio = gpio_num_t::GPIO_NUM_27;
        break;
    case pp::ButtonName::DOWN:
        gpio = gpio_num_t::GPIO_NUM_14;
        break;
    default:
        break;
    }
    return gpio;
}

pp::ButtonName gpioToButton(gpio_num_t _gpio)
{
    pp::ButtonName name = pp::ButtonName::None;
    switch (_gpio)
    {
    case gpio_num_t::GPIO_NUM_25:
        name = pp::ButtonName::RIGHT;
        break;
    case gpio_num_t::GPIO_NUM_26:
        name = pp::ButtonName::UP;
        break;
    case gpio_num_t::GPIO_NUM_27:
        name = pp::ButtonName::LEFT;
        break;
    case gpio_num_t::GPIO_NUM_14:
        name = pp::ButtonName::DOWN;
        break;
    default:
        break;
    }
    return name;
}
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
            input::Button{ .gpio = buttonToGpio(ButtonName::RIGHT) },
            input::Button{ .gpio = buttonToGpio(ButtonName::UP) },
            input::Button{ .gpio = buttonToGpio(ButtonName::LEFT) },
            input::Button{ .gpio = buttonToGpio(ButtonName::DOWN)}
        },
        [queueHandle](const pp::input::Button& _button){
            pp::AppEvent event{
                .button = gpioToButton(_button.gpio),
                .type = pp::AppEvent::Type::ButtonPressed,
            };
            xQueueSend(queueHandle, &event, portMAX_DELAY);
        },
        [queueHandle](const pp::input::Button& _button){
            pp::AppEvent event{
                .button = gpioToButton(_button.gpio),
                .type = pp::AppEvent::Type::ButtonReleased,
            };
            xQueueSend(queueHandle, &event, portMAX_DELAY);
        }
    );

    // TODO CONFIG_ESP32_BUTTON_TASK_STACK_SIZE
    xTaskCreate(&buttonTask, "buttonTask", CONFIG_ESP_MAIN_TASK_STACK_SIZE, &m_inputManager, 10, NULL);    // I could make the stack size smaller
    xTaskCreate(&mainTask, "mainTask", CONFIG_ESP_MAIN_TASK_STACK_SIZE, this, 9, NULL);

    m_state = State::Init;
    transit(StateTransit::Enter);

    // We will return from app_main after initializing everything
}

void App::mainTask(void* _pvParameter)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(CONFIG_PP_TICK_DT_MS);

    App* app = static_cast<App*>(_pvParameter);

    while (true)
    {
        AppEvent event;
        while( xQueueReceive( app->m_queueHandle, &event, 0 ))
        {
            ESP_LOGI(TAG, "handling  %d %d, ", (int)event.type, (int)event.button);

            app->dispatch(event);
        }

        event = AppEvent{ .type = pp::AppEvent::Type::Tick };
        app->dispatch(event);
        
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

void App::dispatch(AppEvent _event)
{
    StateStatus status = dispatchToState(m_state, _event);
    if (status.isTransitRequested())
    {
        PP_ASSERT(m_state != status.nextState, "Cannot transit to the same state");
        transit(StateTransit::Exit);
        m_state = status.nextState;
        transit(StateTransit::Enter);
    }
}

void App::transit(StateTransit _transit)
{
    switch (m_state)
    {
    case State::Init:
        transitInitState(_transit);
        break;
    case State::Menu:
        transitMenuState(_transit);
        break;
    case State::Settings:
        transitSettingsState(_transit);
        break;
    case State::Game:
        transitGameState(_transit);
        break;
    case State::Finish:
        transitFinishState(_transit);
        break;    
    default:
        break;
    }
}

App::StateStatus App::dispatchToState(State _state, AppEvent _event)
{
    StateStatus status;
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

void App::transitInitState(StateTransit _transit)
{
    switch (_transit)
    {
    case StateTransit::Enter:
        m_transitFromInitAfterCount = 20;
        m_display.drawText("    PixelPal", 12, 4);
        break;
    case StateTransit::Exit:
        m_display.clear();
        break;
    }
}

App::StateStatus App::dispatchInitState(AppEvent _event)
{
    StateStatus status;
    switch (_event.type)
    {
    case AppEvent::Type::Tick:
        --m_transitFromInitAfterCount;
        if (m_transitFromInitAfterCount <= 0)
            status = StateStatus{ State::Menu };
        break;
    default:
        break;
    }
    return status;
}

void App::transitMenuState(StateTransit _transit)
{
    switch (_transit)
    {
    case StateTransit::Enter:
        m_display.drawText("Start     <", 11, 3);
        m_display.drawText("Settings   ", 11, 5);
        m_cursorPos = 0;
        break;
    case StateTransit::Exit:
        m_display.clear();
        break;
    }
}

App::StateStatus App::dispatchMenuState(AppEvent _event)
{
    StateStatus status;
    switch (_event.type)
    {
    case AppEvent::Type::ButtonReleased:
        switch (_event.button)
        {
        case ButtonName::DOWN:
            if (m_cursorPos == 0)
            {
                m_cursorPos++;
                m_display.drawText("Start      ", 11, 3);
                m_display.drawText("Settings  <", 11, 5);
            }
            break;
        case ButtonName::UP:
            if (m_cursorPos == 1)
            {
                m_cursorPos--;
                m_display.drawText("Start     <", 11, 3);
                m_display.drawText("Settings   ", 11, 5);
            }
            break;
        case ButtonName::RIGHT:
            status = StateStatus{ m_cursorPos == 0 ? State::Game : State::Settings};
            break;
        default:
            break;
        }
    default:
        break;
    }
    return status;
}

void App::transitSettingsState(StateTransit _transit)
{
    switch (_transit)
    {
    case StateTransit::Enter:
        m_display.drawText("   Settings", 11, 2);
        break;
    case StateTransit::Exit:
        m_display.clear();
        break;
    }
}

App::StateStatus App::dispatchSettingsState(AppEvent _event)
{
    StateStatus status;
    switch (_event.type)
    {
    case AppEvent::Type::ButtonReleased:
        if (_event.button == ButtonName::LEFT)
            status = StateStatus{ State::Menu};
        break;
    default:
        break;
    }
    return status;
}

void App::transitGameState(StateTransit _transit)
{
    switch (_transit)
    {
    case StateTransit::Enter:
        m_game.init(m_display);
        break;
    case StateTransit::Exit:
        m_game.deinit();
        break;
    }
}

App::StateStatus App::dispatchGameState(AppEvent _event)
{
    StateStatus status;
    m_game.dispatch(_event);
    if (m_game.isFinished())
        status = StateStatus{ State::Finish};
    return status;
}

void App::transitFinishState(StateTransit _transit)
{
    switch (_transit)
    {
    case StateTransit::Enter:
        break;
    case StateTransit::Exit:
        break;
    }
}

App::StateStatus App::dispatchFinishState(AppEvent _event)
{
    StateStatus status;
    return status;
}

App::StateStatus::StateStatus(State _nextState)
    : status{StateStatus::Status::Transit}
    , nextState{_nextState}
{
}

} // namespace pp
