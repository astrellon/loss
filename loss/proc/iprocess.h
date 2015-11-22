#pragma once

#include <string>
#include <stdint.h>
#include <chrono>

#include "process_info.h"

#undef val_
#include <boost/coroutine/all.hpp>

namespace loss
{
    class User;
    class Kernel;

    class IProcess
    {
        public:
            IProcess(const std::string &name, const User *user, uint32_t id, Kernel *kernel);

            typedef boost::coroutines::asymmetric_coroutine<void>::pull_type ThreadType;
            typedef boost::coroutines::asymmetric_coroutine<void>::push_type YieldType;

            void resume();
            void run();
            virtual int32_t shutdown();

            ProcessInfo &info();

            void write_std_out(const std::string &message);
            
            typedef std::chrono::steady_clock ClockType;
            std::chrono::time_point<ClockType> finish_time;

            inline bool is_active() const
            {
                return _active;
            }
            inline bool is_running() const
            {
                return _running;
            }

            void check_for_yield();

            void yield();

            virtual int32_t run_impl() = 0;

        private:

            ProcessInfo _info;
            ThreadType *_thread;
            YieldType *_yield;
            bool _active;
            bool _running;
    };
}
