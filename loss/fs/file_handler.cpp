#include "file_handler.h"

#include "ifilesystem.h"
#include "virtual_fileystem.h"

namespace loss
{
    FileHandle::FileHandle(uint32_t entry_id, uint32_t process_id, StreamHandle::OpenMode mode, IFileSystem *fs) :
        StreamHandle(process_id, mode),
        _entry_id(entry_id),
        _fs(fs)
    {
        if (entry_id == 0u)
        {
            throw std::runtime_error("file entry is null");
        }
    }
    
    uint32_t FileHandle::entry_id() const
    {
        return _entry_id;
    }
    IFileSystem *FileHandle::filesystem() const
    {
        return _fs;
    }

    void FileHandle::close()
    {
        _fs->vfs()->close(this);
    }

    IOResult FileHandle::read(uint32_t count, uint8_t *buffer)
    {
        auto result = _fs->read(_entry_id, read_position(), count, buffer);
        change_read_position(result.bytes());
        return result;
    }
    IOResult FileHandle::write(uint32_t count, const uint8_t *data)
    {
        auto result = _fs->write(_entry_id, write_position(), count, data);
        change_write_position(result.bytes());
        return result;
    }

    ReturnCode FileHandle::stream_size(uint32_t &size)
    {
        return _fs->entry_size(_entry_id, size);
    }

    bool FileHandle::at_end_of_stream()
    {
        uint32_t size = 0u;
        auto size_result = stream_size(size);
        if (size_result != SUCCESS)
        {
            return false;
        }
        return read_position() >= static_cast<int32_t>(size);
    
    }
}
