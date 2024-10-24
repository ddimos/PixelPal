#include "game/Player.hpp"
#include "Display.hpp"
#include "AppEvent.hpp"
#include "Constants.hpp"
#include "utils/Assert.hpp"

#include "esp_log.h"

namespace
{
static const char* TAG = "PP_MAP";

constexpr uint8_t PLAYER_BITMAP_WIDTH = 12;
constexpr uint8_t PLAYER_BITMAP_WIDTH_HALF = PLAYER_BITMAP_WIDTH / 2;
constexpr uint8_t PLAYER_BITMAP_HEIGHT = 20;
constexpr uint8_t PLAYER_BITMAP_HEIGHT_HALF = PLAYER_BITMAP_HEIGHT / 2;
constexpr uint8_t PLAYER_BITMAP_SIZE = 36;
constexpr uint8_t NUMBER_OF_PLAYER_BITMAPS = 9;

// player bitmaps, 12x20px
uint8_t playerBitmaps[NUMBER_OF_PLAYER_BITMAPS][PLAYER_BITMAP_SIZE] = 
{
// 'Standing'
{
    0x00, 0x00, 0x7c, 0xfe, 0xf7, 0xbf, 0xbf, 0xf7, 0xfe, 0x7c, 0x00, 0x00, 
    0x04, 0x0e, 0x0e, 0xc4, 0xe5, 0x7f, 0x7f, 0xe5, 0xc4, 0x0e, 0x0e, 0x04, 
    0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x00
},
// 'Moving right 1'
{
    0x00, 0x00, 0x7c, 0xfe, 0xff, 0xf7, 0x3f, 0x3f, 0xf6, 0x7c, 0x00, 0x00, 
    0x00, 0x03, 0x07, 0x86, 0xe5, 0x7f, 0x7f, 0xe5, 0x8c, 0x1c, 0x18, 0x00, 
    0x00, 0x06, 0x06, 0x07, 0x01, 0x00, 0x00, 0x01, 0x07, 0x0e, 0x08, 0x00
},
// 'Moving right 2'
{
    0x00, 0x00, 0x7c, 0xfe, 0xff, 0xf7, 0x3f, 0x3f, 0xf6, 0x7c, 0x00, 0x00, 
    0x04, 0x0e, 0x0e, 0xc4, 0xe5, 0x7f, 0x7f, 0xe5, 0xc4, 0x0e, 0x0e, 0x04, 
    0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x00
},
// 'Moving right 3'
{
    0x00, 0x00, 0x7c, 0xfe, 0xff, 0xf7, 0x3f, 0x3f, 0xf6, 0x7c, 0x00, 0x00, 
    0x00, 0x18, 0x1c, 0x8c, 0xe5, 0x7f, 0x7f, 0xe5, 0x86, 0x07, 0x03, 0x00, 
    0x00, 0x08, 0x0e, 0x07, 0x01, 0x00, 0x06, 0x07, 0x07, 0x00, 0x00, 0x00
},
// 'Moving left 1'
{
    0x00, 0x00, 0x7c, 0xf6, 0x3f, 0x3f, 0xf7, 0xff, 0xfe, 0x7c, 0x00, 0x00, 
    0x00, 0x18, 0x1c, 0x8c, 0xe5, 0x7f, 0x7f, 0xe5, 0x86, 0x07, 0x03, 0x00, 
    0x00, 0x08, 0x0e, 0x07, 0x01, 0x00, 0x00, 0x01, 0x07, 0x06, 0x06, 0x00
},
// 'Moving left 2'
{
    0x00, 0x00, 0x7c, 0xf6, 0x3f, 0x3f, 0xf7, 0xff, 0xfe, 0x7c, 0x00, 0x00, 
    0x04, 0x0e, 0x0e, 0xc4, 0xe5, 0x7f, 0x7f, 0xe5, 0xc4, 0x0e, 0x0e, 0x04, 
    0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x00
},
// 'Moving left 3'
{
    0x00, 0x00, 0x7c, 0xf6, 0x3f, 0x3f, 0xf7, 0xff, 0xfe, 0x7c, 0x00, 0x00, 
    0x00, 0x03, 0x07, 0x86, 0xe5, 0x7f, 0x7f, 0xe5, 0x8c, 0x1c, 0x18, 0x00, 
    0x00, 0x00, 0x00, 0x07, 0x07, 0x06, 0x00, 0x01, 0x07, 0x0e, 0x08, 0x00
},
// 'Moving up/down 1'
{
    0x00, 0x00, 0x7c, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7c, 0x00, 0x00, 
    0x00, 0x18, 0x1c, 0xcc, 0xe5, 0x7f, 0x7f, 0xe5, 0xc6, 0x07, 0x03, 0x00, 
    0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x00
},
// 'Moving up/down 2'
{
    0x00, 0x00, 0x7c, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7c, 0x00, 0x00, 
    0x00, 0x03, 0x07, 0xc6, 0xe5, 0x7f, 0x7f, 0xe5, 0xcc, 0x1c, 0x18, 0x00, 
    0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00
}
};

constexpr uint8_t STANDING_ANIMATION_COUNT = 1;
constexpr uint8_t SIDE_ANIMATION_COUNT = 4;
constexpr uint8_t UP_DOWN_ANIMATION_COUNT = 2;

const uint8_t g_standingAnimationSequence[STANDING_ANIMATION_COUNT] = { 0 };
const uint8_t g_moveRightAnimationSequence[SIDE_ANIMATION_COUNT] = { 2, 1, 2, 3};
const uint8_t g_moveLeftAnimationSequence[SIDE_ANIMATION_COUNT] = { 5, 4, 5, 6 };
const uint8_t g_moveUpDownAnimationSequence[UP_DOWN_ANIMATION_COUNT] = { 7, 8 };
} // namespace

namespace pp::game
{

void Player::dispatch(const AppEvent& _event)
{
    switch (_event.type)
    {
    case AppEvent::Type::ButtonPressed:
    {
        setButton(_event.button);
        break;
    }
    case AppEvent::Type::ButtonReleased:
    {
        clearButton(_event.button);
        break;
    }
    case AppEvent::Type::Tick:
    {
        // delete tick
        break;
    }
    default:
        break;
    }
}

void Player::update()
{
    int8_t directionX = 0;
    int8_t directionY = 0;
    if (isButtonSet(ButtonName::LEFT))
        directionX -= 1;
    if (isButtonSet(ButtonName::RIGHT))
        directionX += 1;
    if (isButtonSet(ButtonName::UP))
        directionY -= 1;
    if (isButtonSet(ButtonName::DOWN))
        directionY += 1;
    
    m_worldX += directionX * TICK_DT_S * m_speed;
    m_worldY += directionY * TICK_DT_S * m_speed;

    if (m_prevDirectionY != directionY || m_prevDirectionX != directionX)
    {
        AnimationState newAnimationState = AnimationState::None;

        if (directionX > 0)
            newAnimationState = AnimationState::LEFT;
        else if (directionX < 0)
            newAnimationState = AnimationState::RIGHT;
        else
        {
            if (directionY > 0)
                newAnimationState = AnimationState::UP;
            else if (directionY < 0)
                newAnimationState = AnimationState::DOWN;
        }

        if (m_animationState != newAnimationState)
            requestAnimation(newAnimationState);
    }

    m_prevDirectionX = directionX;
    m_prevDirectionY = directionY;

    int previousAnimationIndex = m_animationIndex;
    if(m_animationIndex < (m_animationSequenceSize - 1))
        m_animationIndex++;
    else
        m_animationIndex = 0;

    int16_t drawCurrentX = static_cast<int16_t>(m_worldX) % MAP_WIDTH - PLAYER_BITMAP_WIDTH_HALF;
    int16_t drawCurrentY = static_cast<int16_t>(m_worldY) % MAP_HEIGHT - PLAYER_BITMAP_HEIGHT_HALF;

    if (m_drawX != drawCurrentX || m_drawY != drawCurrentY || previousAnimationIndex != m_animationIndex)
    {   
        m_needToDraw = true;

        m_drawX = drawCurrentX;
        m_drawY = drawCurrentY;
        ESP_LOGD(TAG, "dX %d, dY %d, X %f, Y %f", m_drawX, m_drawY, m_worldX, m_worldY);
    }
}

void Player::draw(Display& _displayRef)
{
    PP_ASSERT(m_needToDraw, "Draw shouldn't be called.");
    _displayRef.addBitmap(m_drawX, m_drawY, playerBitmaps[m_animationSequence[m_animationIndex]], PLAYER_BITMAP_WIDTH, PLAYER_BITMAP_SIZE);
}

void Player::reset()
{
    m_worldX = 10.f;
    m_worldY = 14.f;

    m_drawX = 0;
    m_drawY = 0;
    m_speed = 40;

    m_buttonBitmask = 0;
    m_needToDraw = false;

    m_prevDirectionX = 0;
    m_prevDirectionY = 0;

    m_animationIndex = 0;

    requestAnimation(AnimationState::None);
}

void Player::requestAnimation(AnimationState _animationState)
{
    switch (_animationState)
    {
    case AnimationState::UP:
        setAnimation(g_moveUpDownAnimationSequence, UP_DOWN_ANIMATION_COUNT, 1);
        break;
    case AnimationState::DOWN:
        setAnimation(g_moveUpDownAnimationSequence, UP_DOWN_ANIMATION_COUNT);
        break;
    case AnimationState::LEFT:
        setAnimation(g_moveRightAnimationSequence, SIDE_ANIMATION_COUNT);
        break;
    case AnimationState::RIGHT:
        setAnimation(g_moveLeftAnimationSequence, SIDE_ANIMATION_COUNT);
        break;
    default:
        setAnimation(g_standingAnimationSequence, STANDING_ANIMATION_COUNT);
        break;
    }
    m_animationState = _animationState;
}

void Player::setAnimation(const uint8_t* _animationSequence, uint8_t _animationSequenceSize, uint8_t _animationIndex/*= 0*/)
{
    m_animationIndex = _animationIndex;
    m_animationSequence = _animationSequence;
    m_animationSequenceSize = _animationSequenceSize;
}

void Player::setButton(ButtonName _button)
{
    m_buttonBitmask |= 1 << static_cast<int>(_button);
}

void Player::clearButton(ButtonName _button)
{
    m_buttonBitmask &= ~(1 << static_cast<int>(_button));
}

bool Player::isButtonSet(ButtonName _button) const
{
    return (m_buttonBitmask & (1 << static_cast<int>(_button))) != 0;
}

} // namespace pp::game
