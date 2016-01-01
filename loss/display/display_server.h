#pragma once

#include <stdint.h>
#include <vector>
#include <map>
#include <string>
#include <memory>

#include "monitor.h"
#include "desktop.h"

namespace loss
{
    class DisplayServer
    {
        public:
            DisplayServer();

            Monitor *create_monitor(uint32_t width_px, uint32_t height_px);
            Desktop *create_desktop(float width, float height, float xpos, float ypos);
            void attach_desktop(uint32_t desktop_id, uint32_t monitor_id);

            typedef std::map<uint32_t, std::unique_ptr<Monitor> > MonitorMap;
            typedef std::map<uint32_t, std::unique_ptr<Desktop> > DesktopMap;

        private:
            MonitorMap _monitors;
            DesktopMap _desktops;

            uint32_t _desktop_id_counter;
            uint32_t _monitor_id_counter;
    };
}
