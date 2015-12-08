#include "conditional_variable.h"

#include "../kernel.h"

#include <iostream>

namespace loss
{
    ConditionalVariable::ConditionalVariable(Kernel *kernel, WaitCondition wait_cond) :
        ISync(kernel),
        _wait_cond(wait_cond)
    {

    }
    
    void ConditionalVariable::wait()
    {
        while (!_wait_cond())
        {
            wait_current_process();
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
