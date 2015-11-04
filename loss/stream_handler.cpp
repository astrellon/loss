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
    
    IOResult StreamHandle::read_till_character( char terminator, uint32_t max_count, uint8_t *buffer )
    {
        uint8_t temp[256];
        auto counter = 0u;
        auto done = false;
        while (!done)
        {
            auto result = read(64, temp);
            if (result.status() != SUCCESS)
            {
                return result;
            }

            auto read_till = result.bytes();

            for (auto i = 0u; i < result.bytes(); i++)
            {
                if (temp[i] == terminator)
                {
                    done = true;                      
                    read_till = i + 1;
                    break;
                }
            }
            change_read_position(read_till);

            if (result.bytes() < 256)
            {
                done = true;
            }

            for (auto i = 0u; i < read_till && counter < max_count; i++, counter++)
            {
                buffer[counter] = temp[i];
            }
            buffer[counter] = '\0';
        }

        return IOResult(counter, SUCCESS);
    }
    
    IOResult StreamHandle::read_number(double &result)
    {
        auto bytes_read = 0u;
        auto num_counter = 0u;
        
        // Max size of number
        uint8_t num_buff[32];
        uint8_t read_buff[64];

        auto has_period = false;
        while (!at_end_of_stream())
        {
            auto read_result = read(64, read_buff);
            if (read_result.status() != SUCCESS)
            {
                return read_result;
            }
            auto counter = 0u;

            while (isspace(read_buff[counter]))
            {
                counter++;
            }

            if (num_counter == 0u && read_buff[counter] == '-')
            {
                counter++;
                num_buff[num_counter++] = '-';
            }

            for (; counter < read_result.bytes(); counter++)
            {
                if (read_buff[counter] == '.' && !has_period)
                {
                    counter++;
                    has_period = true;
                    num_buff[num_counter++] = '.';
                    continue;
                }

                if (isdigit(read_buff[counter]))
                {
                    num_buff[num_counter++] = read_buff[counter++];
                }
                else
                {
                    break;
                }
            }
            num_buff[num_counter] = '\0';
            bytes_read += counter;
        }

        if (num_counter > 0)
        {
            num_buff[num_counter] = '\0';
            result = atof((const char*)num_buff);

            change_read_position(bytes_read);
            return IOResult(bytes_read, SUCCESS);
        }
        return IOResult(0, INVALID_NUMBER);
    }
    
    IOResult StreamHandle::write_string(const std::string &data)
    {
        return write(data.size(), reinterpret_cast<const uint8_t *>(data.c_str()));
    }
    IOResult StreamHandle::read_stream(uint32_t count, std::ostream &ss)
    {
        // TODO Handle special case of -1 count
        uint8_t *temp = new uint8_t[count];
        auto result = read(count, temp); 
        if (result.status() == SUCCESS && result.bytes() > 0)
        {
            ss.write(reinterpret_cast<const char *>(temp), result.bytes());
        }
        delete []temp;

        change_read_position(result.bytes());
        return result;
    }
    // }}}
}
