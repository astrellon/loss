#include "monitor.h"

namespace loss
{
    Monitor::Monitor(uint32_t width_px, uint32_t height_px, uint32_t id) :
        _width(width_px),
        _height(height_px),
        _id(id),
        _desktop(nullptr)
    {

    }

    uint32_t Monitor::width() const
    {
        return _width;
    }
    uint32_t Monitor::height() const
    {
        return _height;
    }
    uint32_t Monitor::id() const
    {
        return _id;
    }

    Desktop *Monitor::desktop() const
    {
        return _desktop;
    }
    void Monitor::desktop(Desktop *desktop)
    {
        _desktop = desktop;
    }
}
