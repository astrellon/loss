#include "stream_handler.h"
   
namespace loss
{
    // StreamHandle {{{
    StreamHandle::StreamHandle(uint32_t process_id, StreamHandle::OpenMode mode) :
        _process_id(process_id),
        _read_position(0),
        _write_position(0),
        _mode(mode)
    {
        if (process_id == 0u)
        {
            throw std::runtime_error("process is null");
        }
    }
    
    uint32_t StreamHandle::process_id() const
    {
        return _process_id;
    }
    StreamHandle::OpenMode StreamHandle::mode() const
    {
        return _mode;
    }
    bool StreamHandle::has_write_mode() const
    {
        return static_cast<uint32_t>(_mode | StreamHandle::WRITE) > 0;
    }
    bool StreamHandle::has_read_mode() const
    {
        return static_cast<uint32_t>(_mode | StreamHandle::READ) > 0;
    }
            
    int32_t StreamHandle::read_position() const
    {
        return _read_position;
    }
    void StreamHandle::read_position(int32_t pos)
    {
        _read_position = pos;
    }
    void StreamHandle::change_read_position(int32_t change)
    {
        _read_position += change;
    }
            
    int32_t StreamHandle::write_position() const
    {
        return _write_position;
    }
    void StreamHandle::write_position(int32_t pos)
    {
        _write_position = pos;
    }
    void StreamHandle::change_write_position(int32_t change)
    {
        _write_position += change;
    }
    // }}}
}
