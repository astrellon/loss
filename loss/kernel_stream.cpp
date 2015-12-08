#include "kernel_stream.h"

namespace loss
{
    KernelStream::KernelStream(Kernel *kernel) :
        ICharacterDevice(kernel),
        _yield_lock(kernel),
        _yield_cv(kernel, [this]()
        {
            return _data.size() > 0u;
        })
    {

    }

    // Host OS Level {{{
    IOResult KernelStream::read_threaded(uint32_t offset, uint32_t count, uint8_t *buffer)
    {
        if (buffer == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }

        std::unique_lock<std::mutex> lock_guard(_thread_lock);
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
        _yield_lock.unlock();

        return IOResult(read_count, SUCCESS);
    }
    IOResult KernelStream::write_threaded(uint32_t offset, uint32_t count, const uint8_t *data)
    {
        if (data == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }

        std::lock_guard<std::mutex> lock_guard(_thread_lock);
        for (auto i = 0; i < count; i++)
        {
            _data.push_back(data[i]);
        }

        _cv.notify_one();
        _yield_cv.notify_one();
        return IOResult(count, SUCCESS);
    }
    IOResult KernelStream::write_threaded_string(uint32_t offset, const std::string &data)
    {
        return write_threaded(offset, data.size(), (const uint8_t *)data.c_str());
    }
    // }}}
    
    // Loss Kernel Level {{{
    IOResult KernelStream::read(uint32_t offset, uint32_t count, uint8_t *buffer)
    {
        if (buffer == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }

        _yield_cv.wait();
        auto read_count = 0u;
        {
            std::unique_lock<std::mutex> lock_guard(_thread_lock);
            auto max = count;
            if (max > _data.size())
            {
                max = _data.size();
            }
            for (auto i = 0; i < max; i++, read_count++)
            {
                buffer[i] = _data[read_count];
            }

            _data.erase(_data.begin(), _data.begin() + read_count);
        }

        return IOResult(read_count, SUCCESS);
    }
    IOResult KernelStream::write(uint32_t offset, uint32_t count, const uint8_t *data)
    {
        if (data == nullptr)
        {
            return IOResult(0, NULL_PARAMETER);
        }

        std::lock_guard<std::mutex> lock_guard(_thread_lock);
        for (auto i = 0; i < count; i++)
        {
            _data.push_back(data[i]);
        }

        _yield_cv.notify_one();

        return IOResult(count, SUCCESS);
    }
    // }}}

    uint32_t KernelStream::size() const
    {
        return _data.size();
    }
}
