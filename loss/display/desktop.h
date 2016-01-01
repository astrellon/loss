#pragma once

#include <stdint.h>

namespace loss
{
    class Desktop
    {
        public:
            Desktop(float width, float height, uint32_t id);

            uint32_t id() const;

            float width() const;
            void width(float value);
            
            float height() const;
            void height(float value);

            float xpos() const;
            void xpos(float value);
            
            float ypos() const;
            void ypos(float value);

        private:
            float _width;
            float _height;
            float _xpos;
            float _ypos;
            uint32_t _id;
    };
}
