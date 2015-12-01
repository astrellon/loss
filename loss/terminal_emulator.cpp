#include "terminal_emulator.h"

#include <loss/kernel.h>
#include <loss/fs/virtual_fileystem.h>
#include <loss/fs/ifilesystem_entries.h>

#include <iostream>

namespace loss
{
    TerminalEmulator::TerminalEmulator(const User *user, uint32_t id, Kernel *kernel) :
        IProcess("term", user, id, kernel),
        _open(true),
        _file_handle(nullptr),
        _window(nullptr)
    {
    }

    TerminalEmulator::~TerminalEmulator()
    {

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
        if (_file_handle == nullptr)
        {
            return;
        }
        auto yield_counter = 0u;

        auto x = 0u;
        auto y = 0u;
        uint8_t buff[128];
        do
        {
            ++yield_counter;
            if (yield_counter > 8)
            {
                check_for_yield();
                yield_counter = 0u;
            }

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
                    if (y > 50)
                    {
                        y = 10;
                    }
                    else
                    {
                        y++;
                    }
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

    int32_t TerminalEmulator::run_impl()
    {
        _window = initscr();
        if (_window == nullptr)
        {
            std::cout << "Failed to initialise curses window\n";
        }

        noecho();

        scrollok(_window, true);

        refresh();

        auto &vfs = info().vfs();
        vfs.open(1u, "/dev/tty0", loss::FileHandle::WRITE | loss::FileHandle::READ, _file_handle);

        render();
        
        if (_window != nullptr)
        {
            delwin(_window);
            _window = nullptr;
            endwin();
            refresh();
        }

        return 0;
    }
}
