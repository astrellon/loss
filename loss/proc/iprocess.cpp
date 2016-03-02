#include "iprocess.h"

#include "../user.h"
#include "../kernel.h"
#include "../fs/virtual_fileystem.h"

namespace loss
{
    IProcess::IProcess(const std::string &name, const User *user, uint32_t id, Kernel *kernel) :
        _info(name, user, id, kernel),
        _status(NotRunning)
    {

    }

    void IProcess::run()
    {
        _status = Idle;
        _thread = std::unique_ptr<ThreadType>(new ThreadType
        {
            [&](YieldType &yield)
            {
                _status = Running;

                _yield = &yield;
                run_impl();

                _status = Complete;
            }
        });
    }
    int32_t IProcess::shutdown()
    {
        return 0;
    }

    void IProcess::resume()
    {
        (*_thread)();
    }
    void IProcess::yield()
    {
        (*_yield)();
    }

    void IProcess::status(Status status)
    {
        _status = status;
    }
            
    VirtualFileSystem &IProcess::vfs()
    {
        return _info.kernel()->virtual_file_system();
    }

    void IProcess::write_std_out(const std::string &message)
    {
        if (info().std_out() != nullptr)
        {
            info().std_out()->write_string(message);
            //std::cout << message;
        }
    }

    void IProcess::check_for_yield()
    {
        auto now = ClockType::now();
        if (now > finish_time)
        {
            yield();
        }
    }
}
