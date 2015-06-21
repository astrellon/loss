#include "process_manager.h"

#include "../kernel.h"

#include <sstream>

namespace loss
{
    ProcessManager::ProcessManager(Kernel *kernel) :
        _kernel(kernel),
        _id_count(1)
    {

    }

    ReturnCode ProcessManager::create_native_process(const std::string &std_out_path, const std::string &name, const User *user, NativeProcess *& result)
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
    
    ReturnCode ProcessManager::create_lua_process(const std::string &std_out_path, const User *user, LuaProcess *& result)
    {
        auto id = ++_id_count;
        
        FileHandle *std_out_handle = nullptr;
        auto open_result = _kernel->virtual_file_system().open(id, std_out_path, FileHandle::READ, std_out_handle);
        if (open_result != SUCCESS)
        {
            return open_result;
        }

        auto process = new LuaProcess("lua", user, id, _kernel);
        _processes[id] = std::unique_ptr<IProcess>(process);
        result = process;
        process->info().std_out(std_out_handle);

        return SUCCESS;
    }
    ReturnCode ProcessManager::create_process_from_file(const std::string &file_path, const std::string &std_out_path, const User *user, IProcess *&result)
    {
        auto &vfs = _kernel->virtual_file_system();

        FileHandle *file_handle = nullptr;
        auto file_result = vfs.open(result->info().id(), file_path, FileHandle::READ, file_handle);
        if (file_result != SUCCESS)
        {
            return file_result;
        }
        
        LuaProcess *proc = nullptr;
        auto proc_result = create_lua_process(std_out_path, user, proc);
        if (proc_result != SUCCESS)
        {
            return proc_result;
        }

        /* TODO
         * Read first line?
        uint8_t buff[256];
        auto read_result = _kernel->virtual_file_system().read_till_character(file_handle, '\n', 255, buff);
        if (read_result.status() != SUCCESS)
        {
            return read_result.status();
        }
        buff[read_result.bytes()] = '\0';
        */

        std::stringstream file_contents; 
        while (!vfs.at_eof(file_handle))
        {
            vfs.read_stream(file_handle, 4096, file_contents);
        }

        if (proc->load_string(file_contents.str()))
        {
            return FILE_NOT_FOUND;
        }

        result = proc;

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
