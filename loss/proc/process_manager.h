#pragma once

#include <map>
#include <stdint.h>
#include <memory>

#include "iprocess.h"
#include "native_process.h"
#include "lua_process.h"

#include "../return_codes.h"

namespace loss
{
    class Kernel;

    class ProcessManager
    {
        public:
            ProcessManager(Kernel *kernel);

            ReturnCode create_native_process(const std::string &std_out_path, const std::string &name, const User *user, NativeProcess *& result);

            ReturnCode create_lua_process(const std::string &std_out_path, const User *user, LuaProcess *& result);
            ReturnCode create_lua_process_from_file(const std::string &std_out_path, const std::string &file_path, const User *user, LuaProcess *& result);

            ReturnCode delete_process(uint32_t id);
            IProcess *find_process(uint32_t id) const;

            typedef std::map<uint32_t, std::unique_ptr<IProcess> > ProcessMap;
            const ProcessMap &processes() const;
        private:

            Kernel *_kernel;
            ProcessMap _processes;
            uint32_t _id_count;

    };
}
