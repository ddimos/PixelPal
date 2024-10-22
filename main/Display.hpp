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
    void addBitmap(int _x, int _y, const uint8_t* _bitmap, int _width, int _height);
    void getBuffer(uint8_t* _buffer);
    void setBuffer(const uint8_t* _buffer);
    void image(uint8_t* _buffer, uint8_t _w);

    void display();
    void clear();
};

} // namespace pp
