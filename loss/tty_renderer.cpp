#include "tty_renderer.h"

#include "tty.h"

#include <iostream>

extern "C"
{
#include <ncurses.h>
}


namespace loss
{
    TTYRenderer::TTYRenderer() :
        _tty(nullptr),
        _window(nullptr)
    {
        _window = initscr();
        if (_window == nullptr)
        {
            std::cout << "Failed to initialise curses window\n";
        }

        noecho();

        refresh();
    }

    TTYRenderer::~TTYRenderer()
    {
        if (_window != nullptr)
        {
            delwin(_window);
        }
        endwin();
        refresh();
    }

    void TTYRenderer::tty(TTY *tty)
    {
        _tty = tty;
    }
    TTY *TTYRenderer::tty() const
    {
        return _tty;
    }

    void TTYRenderer::render()
    {
        clear();

        if (_tty == nullptr)
        {
            return;
        }

        auto chars = _tty->get();
        auto i = 0u;
        for (auto y = 0u; y < _tty->height(); y++)
        {
            for (auto x = 0u; x < _tty->width(); x++)
            {
                auto c = chars[i++];
                if (c == '\0')
                {
                    continue;
                }
                mvwaddch(_window, y, x, c);
            }
        }

        refresh();
    }
}
