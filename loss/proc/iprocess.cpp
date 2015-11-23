    #include "iprocess.h"

#include "../user.h"
#include "../kernel.h"
#include "../fs/virtual_fileystem.h"

namespace loss
{
    IProcess::IProcess(const std::string &name, const User *user, uint32_t id, Kernel *kernel) :
        _info(name, user, id, kernel),
        _active(true),
        _running(false)
    {

    }

    void IProcess::run()
    {
        _active = true;
        _running = true;

        _thread = new ThreadType
        {
            [&](YieldType &yield)
            {
                _yield = &yield;
                run_impl();

                _active = false;
                _running = false;
            }
        };
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
            
    void IProcess::write_std_out(const std::string &message)
    {
        if (info().std_out() != nullptr)
        {
            info().std_out()->write_string(message);
        }
    }

    void IProcess::check_for_yield()
    {
        auto now = ClockType::now();
        if (now > finish_time)
        {
            //write_std_out("---Yielded---\n");
            yield();
        }
    }
}
