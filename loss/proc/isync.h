#pragma once

#include <stdint.h>

namespace loss
{
    class Kernel;

    class ISync
    {
        public:
            ISync(Kernel *kernel) :
                _kernel(kernel),
                _id(next_id())
            {

            }
            
            inline uint32_t id() const
            {
                return _id;
            }
            inline Kernel *kernel() const
            {
                return _kernel;
            }
            
        private:

            Kernel *_kernel;
            uint32_t _id;

            static uint32_t s_id_counter;
            inline static uint32_t next_id()
            {
                return ++s_id_counter;
            }
    }
}
