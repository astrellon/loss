#include "kernel_manager.h"

#include <loss/fs/ifilesystem.h>

namespace loss
{
    KernelManager::KernelMap KernelManager::s_kernels;
    uint32_t KernelManager::s_id_counter = 0u;

    Kernel *KernelManager::create_new_kernel()
    {
        auto id = ++s_id_counter;
        auto result = new Kernel(id);
        s_kernels[id] = std::unique_ptr<Kernel>(result);
        return result;
    }
    ReturnCode KernelManager::register_kernel(Kernel *kernel)
    {
        if (kernel == nullptr)
        {
            return NULL_PARAMETER;
        }

        const auto &find = s_kernels.find(kernel->id());
        if (find != s_kernels.end())
        {
            if (find->second.get() == kernel)
            {
                return ALREADY_IN_LIST;
            }
            return DIFFERENT_KERNEL_SAME_ID;
        }

        s_kernels[kernel->id()] = std::unique_ptr<Kernel>(kernel);
        return SUCCESS;
    }
    
    Kernel *KernelManager::find_kernel(uint32_t id)
    {
        const auto find = s_kernels.find(id);
        if (find == s_kernels.end())
        {
            return nullptr;
        }

        return find->second.get();
    }

    const KernelManager::KernelMap &KernelManager::kernels()
    {
        return s_kernels;
    }
}
