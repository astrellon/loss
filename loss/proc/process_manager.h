#pragma once

#include <map>
#include <stdint.h>
#include <memory>
#include <queue>
#include <chrono>

#include "iprocess.h"
#include "native_process.h"
#include "lua_process.h"

#include "../return_codes.h"

namespace loss
{
    class Kernel;
    class KernelProcess;

    class ProcessManager
    {
        public:
            ProcessManager(Kernel *kernel);

            ReturnCode create_kernel_proc(const std::string &std_out_path, KernelProcess *&result);
            ReturnCode create_native_process(const std::string &std_out_path, const std::string &name, const User *user, NativeProcess *&result);

            ReturnCode create_lua_process(const std::string &std_out_path, const User *user, LuaProcess *&result);

            ReturnCode create_process_from_file(const std::string &std_out_path, const std::string &file_path, const User *user, IProcess *& result);

            ReturnCode delete_process(uint32_t id);
            IProcess *find_process(uint32_t id) const;

            void run();

            typedef std::map<uint32_t, std::unique_ptr<IProcess> > ProcessMap;
            const ProcessMap &processes() const;

            typedef std::queue<IProcess *> ProcessQueue;
            const ProcessQueue &process_queue() const;

        private:

            Kernel *_kernel;
            uint32_t _id_count;
            bool _running;
            ProcessMap _processes;
            ProcessQueue _process_queue;

            void add_process(IProcess *proc);
    };
}
