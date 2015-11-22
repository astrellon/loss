#pragma once

#include <map>
#include <memory>
#include <stdint.h>
#include <thread>

#include <loss/return_codes.h>
#include "kernel.h"

namespace loss
{
    class KernelManager
    {
        public:
            static Kernel *create_new_kernel();
            static ReturnCode register_kernel(Kernel *kernel, std::thread::id for_thread);
            static Kernel *find_kernel(uint32_t id);
            static Kernel *find_kernel(std::thread::id for_thread);
            static Kernel *get_current_kernel();

            typedef std::map< uint32_t, std::unique_ptr< Kernel > > KernelMap;
            static const KernelMap &kernels();

            typedef std::map<std::thread::id, Kernel *> KernelThreadMap;
            static const KernelThreadMap &kernels_by_thread();

        private:
            static KernelMap s_kernels;
            static KernelThreadMap s_kernels_by_thread;
            static uint32_t s_id_counter;
    };
}
