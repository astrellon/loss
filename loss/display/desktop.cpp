#include "desktop.h"

namespace loss
{
    Desktop::Desktop(float width, float height, uint32_t id) :
        _width(width),
        _height(height),
        _id(id)
    {

    }

    uint32_t Desktop::id() const
    {
        return _id;
    }

    float Desktop::width() const
    {
        return _width; 
    }
    void Desktop::width(float value)
    {
        _width = value;
    }

    float Desktop::height() const
    {
        return _height; 
    }
    void Desktop::height(float value)
    {
        _height = value;
    }

    float Desktop::xpos() const
    {
        return _xpos;
    }
    void Desktop::xpos(float value)
    {
        _xpos = value;
    }

    float Desktop::ypos() const
    {
        return _ypos;
    }
    void Desktop::ypos(float value)
    {
        _ypos = value;
    }

}
