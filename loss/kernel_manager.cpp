#include "kernel_manager.h"

#include <loss/fs/ifilesystem.h>

namespace loss
{
    KernelManager::KernelManager()
    {

    }

    Kernel *KernelManager::create_new_kernel()
    {
        auto id = ++_id_counter;
        auto result = new Kernel(id);
        _kernals[id] = std::unique_ptr<Kernel>(result);
        return result;
    }
    ReturnCode KernelManager::register_kernel(Kernel *kernel)
    {
        if (kernel == nullptr)
        {
            return NULL_PARAMETER;
        }

        const auto &find = _kernals.find(kernel->id());
        if (find != _kernals.end())
        {
            if (find->second.get() == kernel)
            {
                return ALREADY_IN_LIST;
            }
            return DIFFERENT_KERNEL_SAME_ID;
        }

        _kernals[kernel->id()] = std::unique_ptr<Kernel>(kernel);
        return SUCCESS;
    }
    Kernel *KernelManager::find_kernel(uint32_t id) const
    {
        const auto find = _kernals.find(id);
        if (find == _kernals.end())
        {
            return nullptr;
        }

        return find->second.get();
    }

    const KernelManager::KernelMap &KernelManager::kernels() const
    {
        return _kernals;
    }
}
