#include "virtual_fileystem.h"

#include "ifilesystem.h"
#include "ifilesystem_enties.h"

namespace loss
{
    VirtualFileSystem::VirtualFileSystem() :
        _root_filesystem(nullptr)
    {

    }

    void VirtualFileSystem::root_filesystem(IFileSystem *fs)
    {
        _root_filesystem = fs;
    }
    IFileSystem *VirtualFileSystem::root_filesystem() const
    {
        return _root_filesystem;
    }
    IOResult VirtualFileSystem::read(const std::string &name, uint32_t offset, uint32_t count, uint8_t *buffer)
    {
        Path path(name);
        path.dir_to_filename();

        FolderEntry *folder = nullptr;
        auto result = follow_path(path, 1);
        if (result.status() != SUCCESS)
        {
            return IOResult(0, result.status());
        }

        return folder->filesystem()->read(result.id(), path.filename(), offset, count, buffer);
    }
    IOResult VirtualFileSystem::read_stream(const std::string &name, uint32_t offset, uint32_t count, std::ostream &ss)
    {
        uint8_t *temp = new uint8_t[count];
        auto result = read(name, offset, count, temp); 
        if (result.status() == SUCCESS && result.bytes() > 0)
        {
            ss.write(reinterpret_cast<const char *>(temp), result.bytes());
        }
        delete []temp;

        return result;
    }

    IOResult VirtualFileSystem::write(const std::string &name, uint32_t offset, uint32_t count, const uint8_t *data)
    {
        Path path(name);
        path.dir_to_filename();

        FolderEntry *folder = nullptr;
        auto result = follow_path(path, 1);
        if (result.status() != SUCCESS)
        {
            return IOResult(0, result.status());
        }

        return folder->filesystem()->write(result.id(), path.filename(), offset, count, data);
    }
    IOResult VirtualFileSystem::write_string(const std::string &name, uint32_t offset, const std::string &data)
    {
        return write(name, offset, data.size(), reinterpret_cast<const uint8_t *>(data.c_str()));
    }

    ReturnCode VirtualFileSystem::create_folder(const std::string &name)
    {
        Path path(name);
        path.dir_to_filename();

        uint32_t folder_id = 1;
        auto result = follow_path(path, folder_id);
        if (result.status() != SUCCESS)
        {
            return result.status();
        }

        auto fs = result.fs();
        return fs->create_folder(result.id(), path.filename());
    }

    FindFolderResult VirtualFileSystem::follow_path(const Path &path, uint32_t folder_id)
    {
        IFileSystem *fs = _root_filesystem;
        for (auto dir : path.dirs())
        {
            auto result = fs->find_folder(folder_id, dir);
            if (result.status() != SUCCESS)
            {
                return result;
            }
            fs = result.fs();
            folder_id = result.id();
        }

        return FindFolderResult(folder_id, SUCCESS, fs);
    }

}
