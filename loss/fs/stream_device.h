#pragma once

#include "ifilesystem_entries.h"

#include <stdint.h>
#include <vector>

#include "../proc/mutex.h"

namespace loss
{
    class IProcess;

    class StreamDevice : public ICharacterDevice
    {
        public:
            virtual uint32_t size() const;
            virtual IOResult read(uint32_t offset, uint32_t count, uint8_t *buffer);
            virtual IOResult write(uint32_t offset, uint32_t count, const uint8_t *data);

        private:
            std::vector<uint8_t> _data;
            Mutex _mutex;

    };
}
