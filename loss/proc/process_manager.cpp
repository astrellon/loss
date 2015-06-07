#include "process_manager.h"

#include "../kernel.h"

namespace loss
{
    ProcessManager::ProcessManager(Kernel *kernel) :
        _kernel(kernel),
        _id_count(1)
    {

    }

    ReturnCode ProcessManager::create_new_native_process(const std::string &std_out_path, const std::string &name, const User *user, NativeProcess *& result)
    {
        if (name.empty())
        {
            return NULL_PARAMETER;
        }

        auto id = ++_id_count;
        
        FileHandle *std_out_handle = nullptr;
        auto open_result = _kernel->virtual_file_system().open(id, std_out_path, FileHandle::READ, std_out_handle);
        if (open_result != SUCCESS)
        {
            return open_result;
        }

        auto process = new NativeProcess(name, user, id, _kernel);
        _processes[id] = std::unique_ptr<IProcess>(process);
        result = process;
        process->info().std_out(std_out_handle);

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
