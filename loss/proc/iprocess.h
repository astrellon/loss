#pragma once

#include <string>
#include <stdint.h>
#include <chrono>
#include <memory>

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

            enum Status
            {
                NotRunning,
                Idle,
                Running,
                Blocked,
                Complete
            };

            void resume();
            void run();
            virtual int32_t shutdown();

            inline ProcessInfo &info()
            {
                return _info;
            }
            VirtualFileSystem &vfs();

            void write_std_out(const std::string &message);
            
            typedef std::chrono::steady_clock ClockType;
            std::chrono::time_point<ClockType> finish_time;

            inline Status status() const
            {
                return _status;
            }
            void status(Status status);

            void check_for_yield();

            void yield();

            virtual int32_t run_impl() = 0;

        private:

            ProcessInfo _info;
            std::unique_ptr<ThreadType> _thread;
            YieldType *_yield;
            Status _status;
    };
}
