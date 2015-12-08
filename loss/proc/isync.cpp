#include "isync.h"

#include "../kernel.h"
#include "iprocess.h"

namespace loss
{

    uint32_t ISync::s_id_counter = 0u;
    
    void ISync::wait_current_process()
    {
        auto *proc = kernel()->process_manager().current_process();
        kernel()->process_manager().add_blocked_process(id(), proc);
        proc->yield();
    }

}
