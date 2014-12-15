#include "virtual_filesystem.h"

namespace loss
{
    IOResult VirtualFileSystem::read(const std::string &name, uint32_t offset, uint32_t count, uint8_t *buffer)
    {
        std::string test = "Whut up!";
        uint32_t j = 0, i = offset;
        for (; j < count && i < test.size(); i++, j++)
        {
            buffer[j] = test[i];
        }
        if (j < count)
        {
            buffer[j++] = '\0';
        }

        return IOResult(j, SUCCESS);
    }
    IOResult VirtualFileSystem::write(const std::string &name, uint32_t offset, uint32_t count, uint8_t *data)
    {
        return IOResult(0, SUCCESS);
    }

    ReturnCode VirtualFileSystem::getdir(const std::string &name, FolderEntry *to_populate)
    {
        auto entry = new FileEntry();
        entry->size(9);
        return to_populate->add_file("test.txt", entry);
    }
}
