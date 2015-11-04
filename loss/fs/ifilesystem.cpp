#include "ifilesystem.h"

#include "ifilesystem_entries.h"

namespace loss
{
    const uint32_t IFileSystem::ROOT_ID = 1u;
    const uint32_t IFileSystem::NULL_ID = 0u;

    IFileSystem::IFileSystem() :
        _filesystem_id(0u),
        _vfs(nullptr)
    {

    }
    IFileSystem::~IFileSystem()
    {

    }

    void IFileSystem::filesystem_id(uint32_t id)
    {
        _filesystem_id = id;
    }
    uint32_t IFileSystem::filesystem_id() const
    {
        return _filesystem_id;
    }

    VirtualFileSystem *IFileSystem::vfs() const
    {
        return _vfs;
    }
    void IFileSystem::vfs(VirtualFileSystem *vfs)
    {
        _vfs = vfs;
    }

}
