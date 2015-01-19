#pragma once

#include <map>
#include <memory>
#include <stdint.h>

#include <loss/return_codes.h>
#include "kernel.h"

namespace loss
{
    class KernelManager
    {
        public:
            KernelManager();

            Kernel *create_new_kernel();
            ReturnCode register_kernel(Kernel *kernel);
            Kernel *find_kernel(uint32_t id) const;

            typedef std::map< uint32_t, std::unique_ptr< Kernel > > KernelMap;
            const KernelMap &kernels() const;

        private:
            KernelMap _kernals;
            uint32_t _id_counter;
    };
}
