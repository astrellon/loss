#include "mutex.h"

#include "../kernel.h"
#include "../kernel_manager.h"
#include "iprocess.h"

namespace loss
{
    Mutex::Mutex(Kernel *kernel) :
        ISync(kernel),
        _locked(false)
    {

    }

    void Mutex::try_get_lock()
    {
        while (_locked)
        {
            wait();
        }
        _locked = true;
    }
    void Mutex::unlock()
    {
        if (_locked)
        {
            kernel()->process_manager().notify_one_blocked_process(id());
            _locked = false;
        }
    }
    void Mutex::wait()
    {
        auto *proc = kernel()->process_manager().current_process();
        kernel()->process_manager().add_blocked_process(id(), proc);
        proc->yield();
    }
}
