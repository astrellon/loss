#pragma once

//struct _win_st;
//typedef struct _win_st WINDOW;

#undef cast
#undef getstr

#ifdef _WIN32
extern "C"
{
#   include <pdcurses/curses.h>
}
#else
extern "C"
{
#   include <ncurses.h>
}
#endif

#include "proc/iprocess.h"

namespace loss
{
    class Kernel;
    class FileHandle;

    class TerminalEmulator : public IProcess
    {
        public:
            TerminalEmulator(const User *user, uint32_t id, Kernel *kernel);
            ~TerminalEmulator();

            void file_handle(FileHandle *file_handle);
            FileHandle *file_handle() const;

            void is_open(bool open);
            bool is_open() const;

            void render();

            virtual int32_t run_impl();

        private:
            bool _open;
            FileHandle *_file_handle;

            WINDOW *_window;
    };
}
