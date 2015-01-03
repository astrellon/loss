#pragma once

#include <iostream>
#include <map>
#include <stdint.h>

#include <loss/return_codes.h>

namespace loss
{
    class RamFileSystem;

    class RamFileSystemSerialise
    {
        public:

            RamFileSystemSerialise(std::ostream &output, const RamFileSystem *fs);

        private:

            std::map<uint32_t, bool> _serialised_entries;
            std::ostream &_output;
            const RamFileSystem *_fs;
    };
}
