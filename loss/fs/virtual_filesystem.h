#pragma once

#include "ifilesystem.h"
#include <map>
#include <string>

namespace loss
{
    class VirtualFileSystem : public IFileSystem
    {
        public:
            virtual IOResult read(const std::string &name, uint32_t offset, uint32_t count, uint8_t *buffer);
            virtual IOResult write(const std::string &name, uint32_t offset, uint32_t count, uint8_t *data);

            virtual ReturnCode getdir(const std::string &name, FolderEntry *to_populate);
    };
}
