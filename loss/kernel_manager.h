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
            static Kernel *create_new_kernel();
            static ReturnCode register_kernel(Kernel *kernel);
            static Kernel *find_kernel(uint32_t id);

            typedef std::map< uint32_t, std::unique_ptr< Kernel > > KernelMap;
            static const KernelMap &kernels();

        private:
            static KernelMap s_kernels;
            static uint32_t s_id_counter;
    };
}
