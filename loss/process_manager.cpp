#include "process_manager.h"

#include "kernel.h"

namespace loss
{
    ProcessManager::ProcessManager(Kernel *kernel) :
        _kernel(kernel),
        _id_count(0)
    {

    }

    ReturnCode ProcessManager::create_new_native_process(const std::string &name, const User *user, NativeProcess *& result)
    {
        if (user == nullptr || name.empty())
        {
            return NULL_PARAMETER;
        }

        auto id = ++_id_count;
        auto process = new NativeProcess(name, user, id, _kernel);
        _processes[id] = std::unique_ptr<IProcess>(process);
        result = process;

        return SUCCESS;
    }
    ReturnCode ProcessManager::delete_process(uint32_t id)
    {
        const auto find = _processes.find(id);
        if (find == _processes.end())
        {
            return CANNOT_FIND_PROCESS;
        }

        // TODO
        find->second->shutdown();
        _processes.erase(find);
        return SUCCESS;
    }
    IProcess *ProcessManager::find_process(uint32_t id) const
    {
        const auto find = _processes.find(id);
        if (find == _processes.end())
        {
            return nullptr;
        }

        return find->second.get();
           
    }
}
