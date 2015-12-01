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
            ConditionalVariable(Kernel *kernel);

            typedef std::function<bool()> WaitCondition;
            void wait(Mutex &mutex, WaitCondition wait_cond);

            void notify_one();
            void notify_all();

        private:

            Mutex &_mutex;
            WaitCondition _wait_cond;

    };
}
