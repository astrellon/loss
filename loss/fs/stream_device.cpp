#include "stream_device.h"

#include "../return_codes.h"

namespace loss
{
    StreamDevice::StreamDevice(Kernel *kernel) :
        ICharacterDevice(kernel),
        _mutex(kernel)
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
        
        _mutex.try_get_lock();

        auto max = count;
        if (max > _data.size())
        {
            max = _data.size();
        }

        auto read_count = 0u;
        for (auto i = 0u; i < max; i++, read_count++)
        {
            buffer[i] = _data[read_count];
        }

        _data.erase(_data.begin(), _data.begin() + read_count);

        _mutex.unlock();

        return IOResult(read_count, SUCCESS);
    }

    IOResult StreamDevice::write(uint32_t offset, uint32_t count, const uint8_t *data)
    {
        if (data == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }

        _mutex.try_get_lock();
        for (auto i = 0u; i < count; i++)
        {
            _data.push_back(data[i]);
        }
        _mutex.unlock();

        return IOResult(count, SUCCESS);
    }
    /*
    IOResult StreamDevice::read(uint32_t offset, uint32_t count, uint8_t *buffer)
    {
        if (buffer == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }

        std::unique_lock<std::mutex> lock_guard(_lock);
        _cv.wait(lock_guard, [this]()
        {
            return _data.size() > 0u;
        });

        auto max = count;
        if (max > _data.size())
        {
            max = _data.size();
        }

        auto read_count = 0u;
        for (auto i = 0; i < max; i++, read_count++)
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

        std::lock_guard<std::mutex> lock_guard(_lock);
        for (auto i = 0; i < count; i++)
        {
            _data.push_back(data[i]);
        }

        _cv.notify_one();
        return IOResult(count, SUCCESS);
    }
    */
}
