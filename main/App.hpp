#pragma once

#include <freertos/FreeRTOS.h>
#include "freertos/queue.h"

#include "AppEvent.hpp"
#include "Display.hpp"
#include "InputManager.hpp"

namespace pp
{
    class App final
    {
    public:
        App() = default;
        ~App() = default;

        void start();

    private:
        static void mainTask(void* _pvParameter);
        static void buttonTask(void* _pvParameter);

        enum class State : uint8_t
        {
            Init,
            Menu,
            Settings,
            Game,
            Finish
        };
        enum class StateStatus : uint8_t
        {
            Ignored,
            Handled,
            Transit
        };

        enum class StateTransit : uint8_t
        {
            Enter,
            Exit,
        };

        void dispatch(AppEvent _event);
        void transit(State _state, StateTransit _transit);

        StateStatus dispatchToState(State _state, AppEvent _event);
        void transitInitState(StateTransit _transit);
        void transitMenuState(StateTransit _transit);
        void transitSettingsState(StateTransit _transit);
        void transitGameState(StateTransit _transit);
        void transitFinishState(StateTransit _transit);
        StateStatus dispatchInitState(AppEvent _event);
        StateStatus dispatchMenuState(AppEvent _event);
        StateStatus dispatchSettingsState(AppEvent _event);
        StateStatus dispatchGameState(AppEvent _event);
        StateStatus dispatchFinishState(AppEvent _event);

        State m_state = State::Finish;
        int m_transitFromInitAfterCount = 0;
        uint8_t m_cursorPos = 0;

        Display m_display;
        input::InputManager m_inputManager;

        QueueHandle_t m_queueHandle;
    };
    
} // namespace pp
