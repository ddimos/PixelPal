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
    void update();
    void draw(Display& _dispay);

    void reset();

    bool needToDraw() const { return m_needToDraw; }
    
    float getX() const { return m_worldX; }
    float getY() const { return m_worldY; }
    void setX(float _x) { m_worldX = _x; }
    void setY(float _y) { m_worldY = _y; }

private:
    void setAnimation(const uint8_t* _animationSequence, uint8_t _animationSequenceSize, uint8_t _animationIndex = 0);

    float m_worldX = 0.f;
    float m_worldY = 0.f;

    uint8_t m_drawX = 0;
    uint8_t m_drawY = 0;
    uint8_t m_animationIndex = 0;
    const uint8_t* m_animationSequence;
    uint8_t m_animationSequenceSize = 0;
    uint8_t m_speed = 70;

    bool m_moveLeft = false;
    bool m_moveRight = false;
    bool m_moveUp = false;
    bool m_moveDown = false;

    bool m_needToDraw = false;
};

} // namespace pp::game
