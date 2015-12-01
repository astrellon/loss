#include "conditional_variable.h"

namespace loss
{
    ConditionalVariable::ConditionalVariable(Kernel *kernel) :
        ISync(kernel)
    {

    }

    void ConditionalVariable::wait(Mutex &mutex, WaitCondition wait_cond)
    {
        _mutex = mutex;
        _wait_cond = wait_cond;
    }
}
