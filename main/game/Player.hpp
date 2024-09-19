#pragma once
#include "Types.hpp"

namespace pp
{
class Display;
class AppEvent;
}

namespace pp::game
{
class Player
{
public:
    Player() = default;
    ~Player() = default;

    void dispatch(const AppEvent& _event);
    void draw(Display& _dispay);

    void reset();

    bool needToDraw() const { return m_needToDraw; }

private:
    float m_worldX = 0.f;
    float m_worldY = 0.f;

    uint8_t m_drawX = 0;
    uint8_t m_drawY = 0;
    uint8_t m_animationIndex = 0;
    uint8_t m_speed = 50;

    bool m_moveLeft = false;
    bool m_moveRight = false;

    bool m_needToDraw = false;
};

} // namespace pp::game
