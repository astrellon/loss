#pragma once

#include "isync.h"

#include <functional>

namespace loss
{
    class Kernel;

    class ConditionalVariable : public ISync
    {
        public:
            typedef std::function<bool()> WaitCondition;

            ConditionalVariable(Kernel *kernel, WaitCondition wait_cond);

            void wait();
            void notify_one();
            void notify_all();

        private:

            WaitCondition _wait_cond;

    };
}
