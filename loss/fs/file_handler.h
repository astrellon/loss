#pragma once

#include "../stream_handler.h"
#include "../io_common.h"

#include <stdint.h>

namespace loss
{
    class IFileSystem;

    class FileHandle : public StreamHandle
    {
        public:
            FileHandle(uint32_t entry_id, uint32_t process_id, StreamHandle::OpenMode mode, IFileSystem *fs);
            
            uint32_t entry_id() const;
            IFileSystem *filesystem() const;

            virtual void close();

            virtual IOResult read(uint32_t count, uint8_t *buffer);
            virtual IOResult write(uint32_t count, const uint8_t *data);
            virtual ReturnCode stream_size(uint32_t &size);

            virtual bool at_end_of_stream();

        private:
            uint32_t _entry_id;
            IFileSystem *_fs;

    };
}
