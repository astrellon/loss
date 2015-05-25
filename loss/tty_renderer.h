#pragma once

struct _win_st;
typedef struct _win_st WINDOW;

namespace loss
{
    class TTY;

    class TTYRenderer
    {
        public:
            TTYRenderer();
            ~TTYRenderer();

            void tty(TTY *tty);
            TTY *tty() const;

            void render();

        private:
            TTY *_tty;

            WINDOW *_window;
    };
}
