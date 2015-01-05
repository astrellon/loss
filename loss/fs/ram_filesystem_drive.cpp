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

        auto file = dynamic_cast<RamFileSystem::DataFile *>(entry);
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
            write_string(iter->first);
            write_binary(iter->second->id());
        }
    }
    void RamFileSystemSerialise::serialise_file(RamFileSystem::DataFile *file)
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
}
