#pragma once

#include "ifilesystem_entries.h"

#include <stdint.h>
//#include <string>
#include <vector>

#include <pthread.h>

namespace loss
{
    class StreamDevice : public ICharacterDeviceEntry
    {
        public:
            StreamDevice();

            virtual uint32_t size() const;
            virtual IOResult read(uint32_t offset, uint32_t count, uint8_t *buffer);
            virtual IOResult write(uint32_t offset, uint32_t count, const uint8_t *data);

        private:
            //std::stringstream _data;
            std::vector<uint8_t> _data;

    };
}
