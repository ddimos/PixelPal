#pragma once

#include "Map.hpp"
#include "Player.hpp"

namespace pp
{
class Display;
class AppEvent;
}

namespace pp::game
{

class Game
{
public:
    Game() = default;
    ~Game() = default;

    void init(Display& _display);
    void deinit();

    void dispatch(const AppEvent& _event);

    bool isFinished() const { return false; };

private:
    Display* m_displayRef = nullptr;
    Map m_map;
    Player m_player;
};

} // namespace pp::game
