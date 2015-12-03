#include "conditional_variable.h"

#include "../kernel.h"

#include <iostream>

namespace loss
{
    ConditionalVariable::ConditionalVariable(Kernel *kernel, Mutex &lock, WaitCondition wait_cond) :
        ISync(kernel),
        _mutex(lock),
        _wait_cond(wait_cond)
    {

    }
    
    void ConditionalVariable::wait()
    {
        _mutex.try_get_lock();
        while (!_wait_cond())
        {
            _mutex.unlock();

            auto proc = kernel()->process_manager().current_process();
            kernel()->process_manager().add_blocked_process(id(), proc);
            std::cout << "Sleeping condition variable\n";
            proc->yield();

            std::cout << "Waking up from conditon variable\n";
            _mutex.try_get_lock();
        }
    }

    void ConditionalVariable::notify_one()
    {
        kernel()->process_manager().notify_one_blocked_process(id());
    }
    void ConditionalVariable::notify_all()
    {
        kernel()->process_manager().notify_all_blocked_processes(id());
    }
}
