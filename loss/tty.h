#pragma once

#include <stdint.h>

namespace loss
{
    class TTY
    {
        struct Char
        {
            char _char;

            Char (char c = '\0');
        };

        public:
            TTY();
            ~TTY();

            void size(uint32_t width, uint32_t height);
            uint32_t width() const;
            uint32_t height() const;

            void set(uint32_t x, uint32_t y, const Char &c);
            Char get(uint32_t x, uint32_t y) const;
            
            const Char *get() const;

        private:
            uint32_t _width;
            uint32_t _height;

            Char *_buffer;
    };
}
