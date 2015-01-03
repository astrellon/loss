#include "ram_filesystem_drive.h"

#include "ram_filesystem.h"

namespace loss
{
    RamFileSystemSerialise::RamFileSystemSerialise(std::ostream &output, const RamFileSystem *fs) :
        _output(output),
        _fs(fs)
    {

    }
}
