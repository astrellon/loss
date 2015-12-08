#include "mutex.h"

#include "../kernel.h"
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
            wait_current_process();
        }
        _locked = true;
    }
    void Mutex::unlock()
    {
        if (_locked)
        {
            _locked = false;
            kernel()->process_manager().notify_one_blocked_process(id());
        }
    }
}
