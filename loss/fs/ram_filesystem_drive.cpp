#include "ram_filesystem_drive.h"

#include "ram_filesystem.h"

#include <exception>

namespace loss
{
    RamFileSystemSerialise::RamFileSystemSerialise(std::ostream &output, const RamFileSystem *fs) :
        _output(output),
        _fs(fs)
    {
        if (fs == nullptr)
        {
            throw std::runtime_error("");
        }
    }
    void RamFileSystemSerialise::save()
    {
        for (auto iter = _fs->_entry_index.begin(); iter != _fs->_entry_index.end(); ++iter)
        {
            serialise_entry(iter->second.get());
        }
    }

    void RamFileSystemSerialise::serialise_entry(RamFileSystem::Entry *entry)
    {
        auto folder = dynamic_cast<RamFileSystem::Folder *>(entry);
        if (folder != nullptr)
        {
            serialise_folder(folder);
            return;
        }

        auto file = dynamic_cast<RamFileSystem::File *>(entry);
        if (file != nullptr)
        {
            serialise_file(file);
            return;
        }
    }

    void RamFileSystemSerialise::serialise_folder(RamFileSystem::Folder *folder)
    {
        write_binary(folder->id());
        _output << 'f';
        write_binary(folder->size());
        for (auto iter = folder->begin(); iter != folder->end(); ++iter)
        {
            // We don't want to serialise mount points.
            auto mount = dynamic_cast<RamFileSystem::MountPoint *>(iter->second);
            if (mount != nullptr)
            {
                continue;
            }

            // We don't want to serialise character devices.
            auto device = dynamic_cast<RamFileSystem::CharacterDevice *>(iter->second);
            if (device != nullptr)
            {
                continue;
            }

            write_string(iter->first);
            write_binary(iter->second->id());
        }
    }
    void RamFileSystemSerialise::serialise_file(RamFileSystem::File *file)
    {
        write_binary(file->id());
        _output << 'd';
        uint8_t *temp = new uint8_t[file->size()];
        file->read(0, file->size(), temp);
        write_string(temp, file->size());
        delete [] temp;
    }

    void RamFileSystemSerialise::write_string(const std::string &str)
    {
        write_string(str.c_str(), str.size());
    }
    void RamFileSystemSerialise::write_string(const char *str, uint32_t size)
    {
        write_binary(size);
        _output.write(str, size);
    }
    void RamFileSystemSerialise::write_string(const uint8_t *str, uint32_t size)
    {
        write_binary(size);
        _output.write(reinterpret_cast<const char *>(str), size);
    }


    RamFileSystemDeserialise::RamFileSystemDeserialise(std::istream &input, RamFileSystem *fs) :
        _input(input),
        _fs(fs)
    {

    }
    void RamFileSystemDeserialise::load()
    {
        while (_input)
        {
            deserialise_entry();
        }
        finalise();
    }

    void RamFileSystemDeserialise::deserialise_entry()
    {
        auto id = read_binary<uint32_t>();

        char type = '\0';
        _input.read(&type, sizeof(type));

        if (type == 'f')
        {
            deserialise_folder(id);
        }
        else if (type == 'd')
        {
            deserialise_file(id);
        }
    }

    void RamFileSystemDeserialise::deserialise_folder(uint32_t id)
    {
        auto folder = new Folder(id);
        _entries[id] = std::unique_ptr<Entry>(folder);

        auto num_entries = read_binary<uint32_t>();
        for (uint32_t i = 0u; i < num_entries; i++)
        {
            auto entry_name = read_string();
            auto entry_id = read_binary<uint32_t>();
            folder->add_entry(entry_name, entry_id);
        }
    }
    void RamFileSystemDeserialise::deserialise_file(uint32_t id)
    {
        auto size = read_binary<uint32_t>();
        auto data = new uint8_t[size];
        _input.read(reinterpret_cast<char *>(data), size);

        auto file = new File(id, size, data);
        _entries[id] = std::unique_ptr<Entry>(file);
    }

    void RamFileSystemDeserialise::finalise()
    {
        uint32_t max_id = 2;
        std::vector<uint32_t> folders;
        for (auto iter = _entries.begin(); iter != _entries.end(); ++iter)
        {
            if (iter->first > max_id)
            {
                max_id = iter->first;
            }

            auto file_entry = dynamic_cast<File *>(iter->second.get());
            if (file_entry != nullptr)
            {
                auto file_ram_entry = new RamFileSystem::File(iter->first);
                file_ram_entry->write(0, file_entry->size(), file_entry->data());
                _fs->_entry_index[iter->first] = std::unique_ptr<RamFileSystem::Entry>(file_ram_entry);
                continue;
            }

            if (iter->first > 1)
            {
                auto folder_entry = dynamic_cast<Folder *>(iter->second.get());
                if (folder_entry != nullptr)
                {
                    folders.push_back(iter->first);
                    auto folder_ram_entry = new RamFileSystem::Folder(iter->first);
                    _fs->_entry_index[iter->first] = std::unique_ptr<RamFileSystem::Entry>(folder_ram_entry);
                    continue;
                }
            }
            else if (iter->first == 1)
            {
                folders.push_back(1);
            }
        }

        _fs->_id_counter = max_id + 1;

        for (auto id : folders)
        {
            auto ram_folder = dynamic_cast<RamFileSystem::Folder *>(_fs->_entry_index[id].get()); 
            auto folder = dynamic_cast<Folder *>(_entries[id].get());

            auto entries = folder->entries();
            for (auto iter = entries.begin(); iter != entries.end(); ++iter)
            {
                ram_folder->add_entry(iter->first, _fs->_entry_index[iter->second].get());
            }
        }
    }

    std::string RamFileSystemDeserialise::read_string()
    {
        auto size = read_binary<uint32_t>();
        std::string str;
        auto temp = new char[size + 1];
        _input.read(temp, size);
        temp[size] = '\0';
        str.append(temp);
        delete[] temp;

        return str;
    }

    RamFileSystemDeserialise::Entry::Entry(uint32_t id) :
        _id(id)
    {

    }
    RamFileSystemDeserialise::Entry::~Entry()
    {

    }
    uint32_t RamFileSystemDeserialise::Entry::id() const
    {
        return _id;
    }

    RamFileSystemDeserialise::File::File(uint32_t id, uint32_t size, const uint8_t data[]) :
        Entry(id),
        _size(size),
        _data(data)
    {

    }

    uint32_t RamFileSystemDeserialise::File::size() const
    {
        return _size;
    }
    const uint8_t *RamFileSystemDeserialise::File::data() const
    {
        return _data.get();
    }

    RamFileSystemDeserialise::Folder::Folder(uint32_t id) :
        Entry(id)
    {

    }
    void RamFileSystemDeserialise::Folder::add_entry(const std::string &name, uint32_t id)
    {
        _entries[name] = id;
    }
    const RamFileSystemDeserialise::Folder::EntryMap &RamFileSystemDeserialise::Folder::entries() const
    {
        return _entries;
    }
}
