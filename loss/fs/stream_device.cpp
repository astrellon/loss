#include "stream_device.h"

#include <loss/return_codes.h>

namespace loss
{
    StreamDevice::StreamDevice()
    {

    }

    uint32_t StreamDevice::size() const
    {
        return _data.size();
    }

    IOResult StreamDevice::read(uint32_t offset, uint32_t count, uint8_t *buffer)
    {
        if (buffer == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }

        auto max = count;
        if (max > _data.size())
        {
            max = _data.size();
        }

        auto read_count = 0u;
        for (auto i = offset; i < max; i++, read_count++)
        {
            buffer[i] = _data[read_count];
        }

        _data.erase(_data.begin(), _data.begin() + read_count);

        return IOResult(read_count, SUCCESS);
    }

    IOResult StreamDevice::write(uint32_t offset, uint32_t count, const uint8_t *data)
    {
        if (data == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }

        for (auto i = offset; i < offset + count; i++)
        {
            //_data << data[i];
            _data.push_back(data[i]);
        }
        return IOResult(count, SUCCESS);
    }
}
