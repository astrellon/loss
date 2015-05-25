#pragma once

#include <stdint.h>
#include <string>
#include <map>
#include <memory>

namespace loss
{
    // TTY {{{
    class TTY
    {
        public:

            struct CharData
            {
                uint32_t colour_back;
                uint32_t colour_text;

                CharData();
            };

            TTY();
            ~TTY();

            void size(uint32_t width, uint32_t height);
            uint32_t width() const;
            uint32_t height() const;

            void set(uint32_t x, uint32_t y, const char &c);
            char get(uint32_t x, uint32_t y) const;

            void put(const char &c);
            void put_string(const std::string &str);
            
            const char *get() const;
            const CharData *get_data() const;

        private:
            uint32_t _width;
            uint32_t _height;
            uint32_t _cursor_x;
            uint32_t _cursor_y;

            char *_text;
            CharData *_data;

            void clear();
    };
    // }}}

    // TTYManager {{{
    class TTYManager
    {
        public:
            TTYManager();

            typedef std::map<std::string, std::unique_ptr<TTY> > TTYMap;
            const TTYMap *ttys() const;

            TTY *get(const std::string &name);

        private:
            TTYMap _ttys;
    };
    // }}}
}
