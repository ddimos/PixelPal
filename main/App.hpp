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

        enum class State
        {
            Init,
            Menu,
            Settings,
            Game,
            Finish
        };
        enum class StateStatus
        {
            None,
            Ignored,
            Handled,
            Transit
        };
        enum class StateEvent
        {
            None,
            STATE_ENTER,
            STATE_EXIT,
            //
            TICK,
            //
            BUTTON_UP,
            BUTTON_DOWN,
            BUTTON_LEFT,
            BUTTON_RIGHT,
        };

        void dispatch(StateEvent _event);
        StateEvent appToStateEvent(AppEvent _event) const;
        StateStatus dispatchToState(State _state, StateEvent _event);
        StateStatus dispatchInitState(StateEvent _event);
        StateStatus dispatchMenuState(StateEvent _event);
        StateStatus dispatchSettingsState(StateEvent _event);
        StateStatus dispatchGameState(StateEvent _event);
        StateStatus dispatchFinishState(StateEvent _event);

        State m_state = State::Finish;
        int m_transitFromInitAfterCount = 0;
        uint8_t m_cursorPos = 0;

        Display m_display;
        input::InputManager m_inputManager;

        QueueHandle_t m_queueHandle;
    };
    
} // namespace pp
