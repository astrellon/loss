#include "tty.h"

namespace loss
{
    // TTY {{{
    TTY::CharData::CharData() :
        colour_back(0x00000000),
        colour_text(0xFFFFFFFF)
    {

    }

    TTY::TTY() :
        _width(0u),
        _height(0u),
        _cursor_x(0u),
        _cursor_y(0u),
        _text(nullptr),
        _data(nullptr)
    {

    }
    TTY::~TTY()
    {
        clear();
    }

    void TTY::clear()
    {
        if (_text != nullptr)
        {
            delete []_text;
        }
        if (_data != nullptr)
        {
            delete []_data;
        }

        _text = nullptr;
        _data = nullptr;
    }

    void TTY::size(uint32_t width, uint32_t height)
    {
        _width = width;
        _height = height;

        clear();

        auto total = width * height;
        _text = new char[total];
        for (auto i = 0u; i < total; i++)
        {
            _text[i] = '\0';
        }
        _data = new CharData[total];
        for (auto i = 0u; i < total; i++)
        {
            _data[i] = CharData();
        }
    }

    uint32_t TTY::width() const
    {
        return _width;
    }
    uint32_t TTY::height() const
    {
        return _height;
    }

    void TTY::set(uint32_t x, uint32_t y, const char &c)
    {
        _text[x + y * _width] = c;
    }
    char TTY::get(uint32_t x, uint32_t y) const
    {
        return _text[x + y * _width];
    }

    void TTY::put(const char &c)
    {
        _text[_cursor_x + _cursor_y * _width] = c;
        ++_cursor_x;

        if (_cursor_x >= _width)
        {
            ++_cursor_y;
            _cursor_x = 0u;

            // TODO Handle end of buffer;
        }
    }
    void TTY::put_string(const std::string &str)
    {
        for (auto c : str)
        {
            put(c);
        }
    }

    const char *TTY::get() const
    {
        return _text;
    }
    const TTY::CharData *TTY::get_data() const
    {
        return _data;
    }
    // }}}
    
    // TTYManager {{{
    TTYManager::TTYManager()
    {

    }

    const TTYManager::TTYMap *TTYManager::ttys() const
    {
        return &_ttys;
    }

    TTY *TTYManager::get(const std::string &name)
    {
        auto find = _ttys.find(name);
        if (find != _ttys.end())
        {
            return find->second.get();
        }

        auto new_tty = new TTY();
        _ttys[name] = std::unique_ptr<TTY>(new_tty);
        return new_tty;
    }
    // }}}
}
