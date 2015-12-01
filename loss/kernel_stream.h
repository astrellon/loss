#pragma once

#include <vector>
#include <mutex>
#include <stdint.h>

#include "io_common.h"
#include "return_codes.h"
#include "proc/mutex.h"
#include "fs/ifilesystem_entries.h"

#include <mutex>
#include <condition_variable>

namespace loss
{
    class Kernel;

    class KernelStream : public ICharacterDevice
    {
        public:
            KernelStream(Kernel *kernel);

            // Host OS level
            IOResult read_threaded(uint32_t offset, uint32_t count, uint8_t *buffer);
            IOResult write_threaded(uint32_t offset, uint32_t count, const uint8_t *data);
            IOResult write_threaded_string(uint32_t offset, const std::string &data);

            // Loss kernel level
            virtual IOResult read(uint32_t offset, uint32_t count, uint8_t *buffer);
            virtual IOResult write(uint32_t offset, uint32_t count, const uint8_t *data);
            virtual uint32_t size() const;

        private:
            std::vector<uint8_t> _data;

            // Host OS level
            std::mutex _thread_lock;
            std::condition_variable _cv;

            // Loss Kernel level
            Mutex _yield_lock;
    };

}
