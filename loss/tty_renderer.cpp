#include "tty_renderer.h"

#include <loss/kernel.h>
#include <loss/fs/virtual_fileystem.h>
#include <loss/fs/ifilesystem_entries.h>

#include <iostream>

extern "C"
{
#include <ncurses.h>
}


namespace loss
{
    TTYRenderer::TTYRenderer() :
        _open(true),
        _kernel(nullptr),
        _file_handle(nullptr),
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

    void TTYRenderer::kernel(Kernel *kernel)
    {
        _kernel = kernel;
    }
    Kernel *TTYRenderer::kernel() const
    {
        return _kernel;
    }

    void TTYRenderer::file_handle(FileHandle *file_handle)
    {
        _file_handle = file_handle;
    }
    FileHandle *TTYRenderer::file_handle() const
    {
        return _file_handle;
    }

    void TTYRenderer::is_open(bool open)
    {
        _open = open;
    }
    bool TTYRenderer::is_open() const
    {
        return _open;
    }

    void TTYRenderer::render()
    {
        //clear();

        if (_kernel == nullptr || _file_handle == nullptr)
        {
            return;
        }

        auto x = 0u;
        auto y = 0u;
        uint8_t buff[128];
        do
        {
            auto result = _kernel->virtual_file_system().read(_file_handle, 0, 128, buff);
            if (result.status() != SUCCESS)
            {
                std::cout << "Error reading from tty\n";
                _open = false;
            }

            for (auto i = 0u; i < result.bytes(); i++)
            {
                mvwaddch(_window, y, x, static_cast<char>(buff[i]));
                
                x++;
                if (x > 80)
                {
                    x = 0;
                    y++;
                }
            }

            refresh();
        }
        while (_open);

        /*
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
        */

        refresh();
    }
}
