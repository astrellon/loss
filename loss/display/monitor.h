#pragma once

#include <stdint.h>

namespace loss
{
    class Desktop;

    class Monitor
    {
        public:
            Monitor(uint32_t width_px, uint32_t height_px, uint32_t id);

            uint32_t width() const;
            uint32_t height() const;
            uint32_t id() const;

            Desktop *desktop() const;
            void desktop(Desktop *desktop);

        private:
            uint32_t _width;
            uint32_t _height;
            uint32_t _id;
            Desktop *_desktop;

    };
}
