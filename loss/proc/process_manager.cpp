#include "process_manager.h"

#undef val_
#include <boost/coroutine/all.hpp>

#include "kernel_process.h"
#include "../kernel.h"
#include "../terminal_emulator.h"

#include <sstream>
#include <thread>

namespace loss
{
    ProcessManager::ProcessManager(Kernel *kernel) :
        _kernel(kernel),
        _id_count(1),
        _running(false),
        _current_process(nullptr)
    {
    }

    ReturnCode ProcessManager::create_kernel_proc(const std::string &std_out_path, KernelProcess *&result)
    {
        auto id = ++_id_count;
        
        FileHandle *std_out_handle = nullptr;
        auto open_result = _kernel->virtual_file_system().open(id, std_out_path, FileHandle::READ, std_out_handle);
        if (open_result != SUCCESS)
        {
            return open_result;
        }

        auto process = new KernelProcess(id, _kernel);
        add_process(process);
        result = process;
        result->info().std_out(std_out_handle);

        return SUCCESS;
    }
    /*
    ReturnCode ProcessManager::create_native_process(const std::string &std_out_path, const std::string &name, const User *user, NativeProcess *& result)
    {
        if (name.empty())
        {
            return NULL_PARAMETER;
        }

        auto id = ++_id_count;
        
        FileHandle *std_out_handle = nullptr;
        if (std_out_path.size() > 0u)
        {
            auto open_result = _kernel->virtual_file_system().open(id, std_out_path, FileHandle::READ, std_out_handle);
            if (open_result != SUCCESS)
            {
                return open_result;
            }
        }

        auto process = new NativeProcess(name, user, id, _kernel);
        add_process(process);
        result = process;
        process->info().std_out(std_out_handle);

        return SUCCESS;
    }
    */
    ReturnCode ProcessManager::create_native_process(const std::string &std_out_path, const std::string &name, const User *user, IProcess *& result)
    {
        if (name.empty())
        {
            return NULL_PARAMETER;
        }

        auto id = ++_id_count;
        
        FileHandle *std_out_handle = nullptr;
        if (std_out_path.size() > 0u)
        {
            auto open_result = _kernel->virtual_file_system().open(id, std_out_path, FileHandle::READ, std_out_handle);
            if (open_result != SUCCESS)
            {
                return open_result;
            }
        }

        auto process = create_native_process(name, user, id);
        if (process == nullptr)
        {
            return INTERNAL_ERROR;
        }
        add_process(process);
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
        add_process(process);
        result = process;
        process->info().std_out(std_out_handle);

        return SUCCESS;
    }

    ReturnCode ProcessManager::create_process_from_file(const std::string &file_path, const std::string &std_out_path, const User *user, IProcess *&result)
    {
        auto &vfs = _kernel->virtual_file_system();

        FileHandle *file_handle = nullptr;
        auto file_result = vfs.open(_kernel->kernel_proc()->info().id(), file_path, FileHandle::READ, file_handle);
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
        while (!file_handle->at_end_of_stream())
        {
            file_handle->read_stream(4096, file_contents);
        }

        if (!proc->load_string(file_contents.str()))
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

    void ProcessManager::run()
    {
        _running = true;

        while (_running)
        {
            auto proc = _process_queue.front();
            _current_process = proc;
                    
            std::stringstream msg;
            msg << "Running process; " << proc->info().name() << "\n";
            //std::cout << msg.str();
            //_kernel->tty_device()->write_string(msg.str());

            _process_queue.pop();

            proc->finish_time = IProcess::ClockType::now() + std::chrono::microseconds(100); 

            if (proc->status() == IProcess::NotRunning)
            {
                proc->run();
            }
            else if (proc->status() == IProcess::Idle)
            {
                proc->resume();
            }

            if (proc->status() == IProcess::Running || proc->status() == IProcess::Idle)
            {
                _process_queue.push(proc);
            }

            if (_process_queue.size() == 0u)
            {
                if (_process_blocked_map.empty())
                {
                    _running = false;
                }
                else
                {
                    _kernel->tty_device()->write_string("-- Waiting for blocked process");
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                }
            }

        }
    }

    const ProcessManager::ProcessMap &ProcessManager::processes() const
    {
        return _processes;
    }
    const ProcessManager::ProcessQueue &ProcessManager::process_queue() const
    {
        return _process_queue;
    }

    void ProcessManager::add_process(IProcess *proc)
    {
        _processes[proc->info().id()] = std::unique_ptr<IProcess>(proc);
        _process_queue.push(proc);
    }
            
    void ProcessManager::add_blocked_process(uint32_t block_id, IProcess *proc)
    {
        proc->status(IProcess::Blocked);
        _process_blocked_map[block_id].push(proc);
    }
    void ProcessManager::notify_one_blocked_process(uint32_t block_id)
    {
        auto find = _process_blocked_map.find(block_id);
        if (find == _process_blocked_map.end())
        {
            return;
        }

        auto proc = find->second.front();
        std::cout << "Waking up: " << proc->info().name() << "\n";
        proc->status(IProcess::Idle);
        _process_queue.push(proc);
        find->second.pop();

        if (find->second.empty())
        {
            _process_blocked_map.erase(find);
        }
    }
    void ProcessManager::notify_all_blocked_processes(uint32_t block_id)
    {
        auto find = _process_blocked_map.find(block_id);
        if (find == _process_blocked_map.end())
        {
            return;
        }

        while (!find->second.empty())
        {
            auto proc = find->second.front();
            proc->status(IProcess::Idle);
            _process_queue.push(proc);
            find->second.pop();
        }
        
        if (find->second.empty())
        {
            _process_blocked_map.erase(find);
        }
    }

    IProcess *ProcessManager::create_native_process(const std::string &name, const User *user, uint32_t id)
    {
        if (name == "term")
        {
            return new TerminalEmulator(user, id, _kernel);
        }
        return nullptr;
    }
}
