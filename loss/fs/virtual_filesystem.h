#pragma once

#include "ifilesystem.h"

namespace loss
{
    class VirtualFileSystem : public IFileSystem
    {
        public:
            virtual IOResult read(const std::string &name, uint32_t offset, uint32_t count, uint8_t *buffer);
            virtual IOResult write(const std::string &name, uint32_t offset, uint32_t counter, uint8_t *data);

            virtual ReturnCode getdir(const std::string &name, IFolder *to_populate);
    };
}
