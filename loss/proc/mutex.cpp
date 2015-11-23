#include "mutex.h"

#include "../kernel.h"
#include "../kernel_manager.h"
#include "iprocess.h"

namespace loss
{
    uint32_t Mutex::s_id_counter = 0u;

    Mutex::Mutex(Kernel *kernel) :
        _id(next_id()),
        _locked(false),
        _kernel(kernel)
    {

    }

    void Mutex::try_get_lock()
    {
        while (_locked)
        {
            auto *proc = _kernel->process_manager().current_process();
            _kernel->process_manager().add_blocked_process(_id, proc);
            proc->yield();
        }
        _locked = true;
    }
    void Mutex::unlock()
    {
        if (_locked)
        {
            _kernel->process_manager().notify_one_blocked_process(_id);
            _locked = false;
        }
    }
}
