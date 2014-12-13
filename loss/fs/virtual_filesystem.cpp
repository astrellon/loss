#include "virtual_filesystem.h"

#include "inode.h"

namespace loss
{

    IMetadata &VirtualFile::metadata()
    {
        return _metadata;
    }
    const IMetadata &VirtualFile::metadata() const
    {
        return _metadata;
    }

    
}
