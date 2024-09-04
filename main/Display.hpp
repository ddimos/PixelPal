#pragma once

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
    void clear();
};

} // namespace pp
