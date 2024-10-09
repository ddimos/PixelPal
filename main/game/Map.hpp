#pragma once

#include <array>
#include <memory>

namespace pp
{
class Display;
}

namespace pp::game
{
class Player;

enum PreloadedZone : uint8_t
{
    CENTER = 0,
    UP,
    RIGHT,
    DOWN,
    LEFT
};

constexpr uint8_t NUMBER_OF_PRELOADED_ZONES = 2;

class Map
{
public:
    Map();
    ~Map() = default;

    void init(Display& _display);
    void deinit();

    void update(Player& _player);
    void draw(Display& _display);

private:
    void preloadZones(PreloadedZone _nextPreloadedZone);
    void preloadZone(uint8_t _nextZoneNumber, uint8_t _nextPreloadedZoneIndex);
    int getNextZoneNumber(PreloadedZone _nextPreloadedZone) const;

    uint8_t m_currentZoneNumber = 255;
    uint8_t m_previousZoneNumber = 255;
    // Use of std::unique_ptr increases IROM  only by ~100B.
    std::array<std::unique_ptr<uint8_t[]>, NUMBER_OF_PRELOADED_ZONES> m_preloadedZones;
    uint8_t m_currentZoneIndex = 0;
    uint8_t m_previousZoneIndex = 1;
};

} // namespace pp::game
