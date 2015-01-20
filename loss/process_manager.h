#pragma once

#include "iprocess.h"
#include "native_process.h"
#include <map>
#include <stdint.h>
#include <memory>

#include <loss/return_codes.h>

namespace loss
{
    class ProcessManager
    {
        public:
            ProcessManager();

            ReturnCode create_new_native_process(const std::string &name, const User *user, NativeProcess *& result);
            ReturnCode delete_process(uint32_t id);
            IProcess *find_process(uint32_t id) const;

            typedef std::map<uint32_t, std::unique_ptr<IProcess> > ProcessMap;
            const ProcessMap &processes() const;
        private:

            ProcessMap _processes;
            uint32_t _id_count;

    };
}
