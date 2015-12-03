#pragma once

#include "isync.h"
#include "mutex.h"

#include <functional>

namespace loss
{
    class Kernel;

    class ConditionalVariable : public ISync
    {
        public:
            typedef std::function<bool()> WaitCondition;

            ConditionalVariable(Kernel *kernel, Mutex &lock, WaitCondition wait_cond);

            void wait();
            void notify_one();
            void notify_all();

        private:

            Mutex &_mutex;
            WaitCondition _wait_cond;

    };
}
