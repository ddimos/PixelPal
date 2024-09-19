#include "game/Game.hpp"
#include "Display.hpp"
#include "AppEvent.hpp"
#include "Constants.hpp"

namespace
{


} // namespace


namespace pp::game
{

void Game::init(Display& _display)
{
    m_displayRef = &_display;
    m_player.reset();
    m_map.init(_display);
}

void Game::deinit()
{
    m_map.deinit();
}

void Game::dispatch(const AppEvent& _event)
{
    m_player.dispatch(_event);

    switch (_event.type)
    {
    case AppEvent::Type::Tick:
    {
        if (m_player.needToDraw())
        {
            m_map.draw(*m_displayRef);
            m_player.draw(*m_displayRef);

            m_displayRef->display();
        }
        break;
    }
    default:
        break;
    }
}

} // namespace pp::game
