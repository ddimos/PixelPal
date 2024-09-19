#pragma once

#include <memory>

namespace pp
{
class Display;
}

namespace pp::game
{

class Map
{
public:
    Map();
    ~Map() = default;

    void init(Display& _display);
    void deinit();

    void draw(Display& _display);

private:
    // Use of std::unique_ptr increases IROM  only by ~100B.
    std::unique_ptr<uint8_t[]> m_drawBuffer;
};

} // namespace pp::game
