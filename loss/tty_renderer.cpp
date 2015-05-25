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

        mvwaddstr(_window, 1, 4, "Move the window");
        mvwaddstr(_window, 2, 2, "with the arrow keys");
        mvwaddstr(_window, 3, 6, "or HOME/END");
        mvwaddstr(_window, 5, 3, "Press 'q' to quit");

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

    }
}
