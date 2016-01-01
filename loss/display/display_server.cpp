#include "display_server.h"

namespace loss
{
    DisplayServer::DisplayServer() :
        _desktop_id_counter(0u),
        _monitor_id_counter(0u)
    {

    }

    Monitor *DisplayServer::create_monitor(uint32_t width_px, uint32_t height_px)
    {
        auto id = ++_monitor_id_counter;
        auto new_monitor = new Monitor(width_px, height_px, id);
        _monitors[id] = std::unique_ptr<Monitor>(new_monitor);
        return new_monitor;
    }
    Desktop *DisplayServer::create_desktop(float width, float height, float xpos, float ypos)
    {
        auto id = ++_desktop_id_counter;
        auto new_desktop = new Desktop(width, height, id);
        _desktops[id] = std::unique_ptr<Desktop>(new_desktop);
        return new_desktop;
    }

    void DisplayServer::attach_desktop(uint32_t desktop_id, uint32_t monitor_id)
    {
        auto find_monitor = _monitors.find(monitor_id);
        if (find_monitor == _monitors.end())
        {
            return;
        }

        auto find_desktop = _desktops.find(desktop_id);
        if (find_desktop == _desktops.end())
        {
            return;
        }

        find_monitor->second->desktop(find_desktop->second.get());
    }
}
