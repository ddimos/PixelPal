#pragma once

namespace pp
{
class Display;
class AppEvent;
}

namespace pp::game
{

struct Player
{
    float x_f = 0.f;
    float y_f = 0.f;

    int x_previousDraw = 0;
    int y_previousDraw = 0;
    int animationIndex = 0;
    int x_speed = 50;

    bool moveLeft = false;
    bool moveRight = false;

    void reset();
};

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
    Player m_player;
};

} // namespace pp::game
