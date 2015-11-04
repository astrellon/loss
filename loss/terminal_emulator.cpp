#include "terminal_emulator.h"

#include <loss/kernel.h>
#include <loss/fs/virtual_fileystem.h>
#include <loss/fs/ifilesystem_entries.h>

#include <iostream>

namespace loss
{
    TerminalEmulator::TerminalEmulator() :
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

    TerminalEmulator::~TerminalEmulator()
    {
        if (_window != nullptr)
        {
            delwin(_window);
        }
        endwin();
        refresh();
    }

    void TerminalEmulator::kernel(Kernel *kernel)
    {
        _kernel = kernel;
    }
    Kernel *TerminalEmulator::kernel() const
    {
        return _kernel;
    }

    void TerminalEmulator::file_handle(FileHandle *file_handle)
    {
        _file_handle = file_handle;
    }
    FileHandle *TerminalEmulator::file_handle() const
    {
        return _file_handle;
    }

    void TerminalEmulator::is_open(bool open)
    {
        _open = open;
    }
    bool TerminalEmulator::is_open() const
    {
        return _open;
    }

    void TerminalEmulator::render()
    {
        if (_kernel == nullptr || _file_handle == nullptr)
        {
            return;
        }

        auto x = 0u;
        auto y = 0u;
        uint8_t buff[128];
        do
        {
            auto result = _file_handle->read(128, buff);
            if (result.status() != SUCCESS)
            {
                std::cout << "Error reading from tty\n";
                _open = false;
            }
            _file_handle->read_position(0u);

            for (auto i = 0u; i < result.bytes(); i++)
            {
                auto c = static_cast<char>(buff[i]);
                if (c == '\n')
                {
                    y++;
                    x = 0;
                }
                else if (c == '\r')
                {
                    x = 0;
                }
                else
                {
                    mvwaddch(_window, y, x, c);
                    x++;
                    if (x > 80)
                    {
                        x = 0;
                        y++;
                    }
                }
                
            }

            refresh();
        }
        while (_open);

        refresh();
    }
}
