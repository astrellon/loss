#include "proc_filesystem.h"

#include "../kernel.h"
#include "file_handler.h"

#include <sstream>

namespace loss
{
    ProcFileSystem::ProcFileSystem(Kernel *kernel) :
        RamFileSystem(),
        _kernel(kernel)
    {
        init_base();
    }

    Kernel *ProcFileSystem::kernel() const
    {
        return _kernel;
    }

    void ProcFileSystem::init_base()
    {
        _current_proc_id = next_id();

        auto handle = create_file_handle("cpuinfo");
        handle->write_string("CPU: Best\nMem: Lots\n");
        delete handle;

        handle = create_file_handle("meminfo");
        handle->write_string("Mem slot 0: Lots\n");
        delete handle;
    }
    void ProcFileSystem::update_processes()
    {
        
    }

    ReturnCode ProcFileSystem::read_folder(uint32_t process_id, uint32_t folder_id, FolderEntry &to_populate)
    {
        if (folder_id  == ROOT_ID)
        {
            auto result = RamFileSystem::read_folder(process_id, folder_id, to_populate);
            if (result != SUCCESS)
            {
                return result;
            } 

            for (auto &iter : _kernel->process_manager().processes())
            {
                auto folder = new FolderEntry(folder_id, this);
                folder->id(proc_folder_id(iter.first));
                std::stringstream folder_name;
                folder_name << iter.first;
                to_populate.add_entry(folder_name.str(), folder);
            }

            {
                auto folder = new FolderEntry(folder_id, this);
                folder->id(_current_proc_id);
                to_populate.add_entry("current", folder);
            }

            return SUCCESS;
        }
        else if (folder_id == _current_proc_id)
        {

        }

        return SUCCESS;
    }

    FileHandle *ProcFileSystem::create_file_handle(const std::string &filename)
    {
        auto entry = create_file(ROOT_ID, "cpuinfo");
        return new FileHandle(entry.id(), _kernel->kernel_proc()->info().id(), FileHandle::WRITE, this);
    }

    uint32_t ProcFileSystem::proc_folder_id(uint32_t process_id)
    {
        auto find = _proc_folder_map.find(process_id);
        if (find != _proc_folder_map.end())
        {
            return find->second;
        }

        auto id = next_id();
        _proc_folder_map[process_id] = id;
        return id;
    }
}
