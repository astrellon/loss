#pragma once

#include <stdint.h>

namespace loss
{
    class Mutex
    {
        public:
            Mutex();

            inline uint32_t id() const
            {
                return _id;
            }
            inline bool is_locked() const
            {
                return _locked;
            }
            inline void is_locked(bool value)
            {
                _locked = value;
            }

            void try_get_lock();
            void unlock();

        private:
            uint32_t _id;
            bool _locked;
            
            static uint32_t s_id_counter;
            inline static uint32_t next_id()
            {
                return ++s_id_counter;
            }

    };
}
