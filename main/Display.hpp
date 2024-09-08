#pragma once
#include <stdint.h>

namespace pp
{
class Display final
{
public:
    Display() = default;
    ~Display() = default;

    void init();
    void deinit();

    void drawText(const char* _text, int _length, int _pos);
    void drawBitmap(int _x, int _y, uint8_t* _bitmap, int _width, int _height);

    void clear();
};

} // namespace pp
