#include "tty.h"

namespace loss
{
    TTY::Char::Char(char c) :
        _char(c)
    {

    }

    TTY::TTY() :
        _width(0u),
        _height(0u),
        _buffer(nullptr)
    {

    }
    TTY::~TTY()
    {
        if (_buffer != nullptr)
        {
            delete []_buffer;
        }
    }

    void TTY::size(uint32_t width, uint32_t height)
    {
        _width = width;
        _height = height;

        if (_buffer != nullptr)
        {
            delete []_buffer;
        }
        _buffer = new Char[width * height];
    }

    uint32_t TTY::width() const
    {
        return _width;
    }
    uint32_t TTY::height() const
    {
        return _height;
    }

    void TTY::set(uint32_t x, uint32_t y, const Char &c)
    {
        _buffer[x + y * _width] = c;
    }
    TTY::Char TTY::get(uint32_t x, uint32_t y) const
    {
        return _buffer[x + y * _width];
    }

    const TTY::Char *TTY::get() const
    {
        return _buffer;
    }
}
