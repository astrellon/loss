#pragma once

#include "isync.h"

namespace loss
{
    class Kernel;

    class Mutex : public ISync
    {
        public:
            Mutex(Kernel *kernel);

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
            void wait();

        private:
            bool _locked;

    };
}
