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
    using AnimationState = ButtonName;

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
    
    void requestAnimation(AnimationState _animationState);
    void setAnimation(const uint8_t* _animationSequence, uint8_t _animationSequenceSize, uint8_t _animationIndex = 0);
    void setButton(ButtonName _button);
    void clearButton(ButtonName _button);
    bool isButtonSet(ButtonName _button) const;

    float m_worldX = 0.f;
    float m_worldY = 0.f;

    int16_t m_drawX = 0;
    int16_t m_drawY = 0;
    uint8_t m_speed = 40;

    int8_t m_prevDirectionX = 0;
    int8_t m_prevDirectionY = 0;

    static_assert(sizeof(ButtonName) == sizeof(uint8_t), "The button bitmask is too small");
    uint8_t m_buttonBitmask = 0;

    bool m_needToDraw = false;

    uint8_t m_animationIndex = 0;
    const uint8_t* m_animationSequence;
    uint8_t m_animationSequenceSize = 0;
    AnimationState m_animationState = AnimationState::None;
};

} // namespace pp::game
