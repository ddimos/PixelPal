#include "game/Map.hpp"
#include "Display.hpp"
#include "Types.hpp"
#include "Player.hpp"
#include "utils/Assert.hpp"
#include "Constants.hpp"

#include <cstring>
#include "esp_log.h"

namespace
{
static const char* TAG = "PP_MAP";

constexpr unsigned NUMBER_OF_BYTES_PER_ZONE = pp::MAP_WIDTH * pp::MAP_HEIGHT / 8;

constexpr unsigned NUMBER_OF_ZONES_WIDTH = 3;
constexpr unsigned NUMBER_OF_ZONES_HEIGHT = 3;
constexpr unsigned NUMBER_OF_ZONES = NUMBER_OF_ZONES_WIDTH * NUMBER_OF_ZONES_HEIGHT;

constexpr unsigned NUMBER_OF_MAP_ELEMENT_TYPES = 9;
constexpr unsigned MAP_ELEMENT_WIDTH = 16;
constexpr unsigned MAP_ELEMENT_HEIGHT = 8;
constexpr unsigned NUMBER_OF_MAP_ELEMENTS_PER_ZONE = (pp::MAP_WIDTH / MAP_ELEMENT_WIDTH) * (pp::MAP_HEIGHT / MAP_ELEMENT_HEIGHT);

static_assert(NUMBER_OF_ZONES < 255, "Change type of the zone number");

// width=3, height=3
// [00][01][02]
// [03][04][05]
// [06][07][08]

// width=5, height=3
// [00][01][02][03][04]
// [05][06][07][08][09]
// [10][11][12][13][14]


enum MapElementType : uint8_t
{
    E = 0,  // EMPTY
    L,      // LEFT
    R,      // RIGHT
    T,      // TOP
    B,      // BOTTOM
    LT,     // LEFT TOP
    RT,     // RIGHT TOP
    LB,     // LEFT BOTTOM
    RB,     // RIGHT BOTTOM
    // C    // CENTER WALL
    // X    // EXIT
    // ST   // STAIRS
    // EB   // EMTY BOTTOM
    // ET   // EMTY TOP
    Count
};

static_assert(static_cast<int>(MapElementType::Count) == NUMBER_OF_MAP_ELEMENT_TYPES);

const uint8_t g_mapElements[NUMBER_OF_MAP_ELEMENT_TYPES][MAP_ELEMENT_WIDTH] = 
{
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // E
{ 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // L
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff }, // R
{ 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f }, // T
{ 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0 }, // B

{ 0xff, 0xff, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f }, // LT
{ 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0xff, 0xff }, // RT
{ 0xff, 0xff, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0 }, // LB
{ 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xff, 0xff }  // RB
};

// TODO It can be only 3-4 bits
const std::array<std::array<MapElementType, NUMBER_OF_MAP_ELEMENTS_PER_ZONE>, NUMBER_OF_ZONES> g_zones = 
{
{
{
    LT, T , T , T , T , T , T , T ,
    L , E , E , E , E , E , E , E ,
    L , E , E , E , E , E , E , E ,
    LB, B , B , B , B , B , E , B ,
    LT, T , T , RT, T , T , E , T ,
    L , E , E , R , E , E , E , E ,
    L , E , E , R , E , E , E , E ,
    LB, E , B , RB, B , B , B , B 
},
{
    T , T , T , E , E , T , RT, T ,
    E , E , E , E , E , E , R , E ,
    E , E , E , E , E , E , R , E ,
    B , B , B , B , B , B , RB, B ,
    T , T , T , T , T , T , T , T ,
    E , E , E , E , E , E , E , E ,
    E , E , E , E , E , E , E , E ,
    B , B , B , B , B , B , E , B 
},
{
    T , T , T , T , T , T , T , RT,
    E , E , E , E , E , E , E , R ,
    E , E , E , E , E , E , E , R ,
    B , B , E , B , B , E , B , RB,
    T , T , E , T , LT, E , T , RT,
    E , E , E , E , L , E , E , R ,
    E , E , E , E , L , E , E , R ,
    B , B , B , B , LB, B , E , RB 
},
{
    LT, E , T , T , T , RT, T , T,
    L , E , E , E , E , R , E , E ,
    L , E , E , E , E , R , E , E ,
    LB, B , E , B , B , RB, E , B,
    LT, T , E , T , T , LT, E , T,
    L , E , E , E , E , L , E , E ,
    L , E , E , E , E , L , E , E ,
    LB, E , B , B , B , LB, B , B 
},
{
    T , T , T , T , T , T , E , T ,
    E , E , E , E , E , E , E , E ,
    E , E , E , E , E , E , E , E ,
    B , B , B , B , B , B , B , B ,
    T , T , T , T , T , T , T , T ,
    E , E , E , E , E , E , E , E ,
    E , E , E , E , E , E , E , E ,
    B , B , B , B , B , E , B , B 
},
{
    T , T , T , T , T , T , E , RT,
    E , E , E , E , E , E , E , R ,
    E , E , E , E , E , E , E , R ,
    B , B , B , B , E , B , B , RB,
    T , T , T , T , E , T , T , RT,
    E , E , E , E , E , E , E , R ,
    E , E , E , E , E , E , E , R ,
    B , B , B , B , B , B , B , RB 
},
{
    LT, E , T , T , T , T , T , T ,
    L , E , E , E , E , E , E , E ,
    L , E , E , E , E , E , E , E ,
    LB, B , B , B , B , B , E , B ,
    LT, T , T , T , T , T , E , T,
    L , E , E , E , E , E , E , E ,
    L , E , E , E , E , E , E , E ,
    LB, B , B , B , B , B , B , B 
},
{
    T , RT, T , T , T , E , T , T ,
    E , R , E , E , E , E , E , E ,
    E , R , E , E , E , E , E , E ,
    B , RB, E , B , B , B , B , B ,
    T , T , E , T , T , T , T , T ,
    E , E , E , E , E , E , E , E ,
    E , E , E , E , E , E , E , E ,
    B , B , B , B , B , B , B , B 
},
{
    T , T , T , T , T , T , T , RT,
    E , E , E , E , E , E , E , R ,
    E , E , E , E , E , E , E , R ,
    B , B , E , B , B , B , E , RB,
    T , T , E , T , T , T , E , RT,
    E , E , E , E , E , E , E , R ,
    E , E , E , E , E , E , E , R ,
    B , B , B , B , B , B , B , RB 
}
}
};

} // namespace

namespace pp::game
{

Map::Map()
{
}

void Map::init(Display& _display)
{
    for(auto& zone : m_preloadedZones)
    {
        zone = std::make_unique<uint8_t[]>(NUMBER_OF_BYTES_PER_ZONE);
    }

    m_currentZoneNumber = 0;
    m_currentZoneIndex = 0;
    preloadZone(m_currentZoneNumber, m_currentZoneIndex);
}

void Map::deinit()
{
    for(auto& zone : m_preloadedZones)
    {
        zone.reset();
    }
}

void Map::update(Player& _player)
{
    float x = _player.getX(), y = _player.getY();
    PP_ASSERT((x < 0.f || y < 0.f), "World position can't be less than zero");
    uint8_t ix = static_cast<int>(x) / MAP_WIDTH;
    uint8_t iy = static_cast<int>(y) / MAP_HEIGHT;

    uint8_t nextZoneNumber = ix + iy * NUMBER_OF_ZONES_WIDTH;
    if (m_currentZoneNumber != nextZoneNumber)
    {
        ESP_LOGI(TAG, "preloadZones nextZone %d, current %d", nextZoneNumber, m_currentZoneNumber);

        if (m_previousZoneNumber == nextZoneNumber)
        {
            std::swap(m_previousZoneNumber, m_currentZoneNumber);
            std::swap(m_previousZoneIndex, m_currentZoneIndex);
            return;
        }

        preloadZone(nextZoneNumber, m_previousZoneIndex);

        std::swap(m_previousZoneIndex, m_currentZoneIndex);
        m_previousZoneNumber = m_currentZoneNumber;
        m_currentZoneNumber = nextZoneNumber;
    }
}

void Map::draw(Display& _display)
{
    _display.setBuffer(m_preloadedZones[m_currentZoneIndex].get());
}

void Map::preloadZone(uint8_t _nextZoneNumber, uint8_t _nextPreloadedZoneIndex)
{
    PP_ASSERT(_nextZoneNumber < NUMBER_OF_ZONES, "Invalid value of next zone number.");
    PP_ASSERT(_nextPreloadedZoneIndex < NUMBER_OF_PRELOADED_ZONES, "Invalid value of next preloaded zone index.");

    auto& zone = g_zones[_nextZoneNumber];
    unsigned i = 0;
    for(MapElementType mapElement : zone)
    {
        std::memcpy(
            m_preloadedZones[_nextPreloadedZoneIndex].get() + i * MAP_ELEMENT_WIDTH,
            g_mapElements[static_cast<uint8_t>(mapElement)], MAP_ELEMENT_WIDTH);
        ++i;
    }
}
} // namespace pp::game
