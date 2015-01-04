#pragma once

#include <iostream>
#include <map>
#include <stdint.h>

#include <loss/return_codes.h>
#include <loss/fs/ram_filesystem.h>

namespace loss
{
    class RamFileSystemSerialise
    {
        public:

            RamFileSystemSerialise(std::ostream &output, const RamFileSystem *fs);
            void save();

        private:

            std::ostream &_output;
            const RamFileSystem *_fs;

            void serialise_entry(RamFileSystem::Entry *entry);
            void serialise_folder(RamFileSystem::Folder *folder);
            void serialise_file(RamFileSystem::DataFile *file);

            template <class T>
            void write_binary(T value)
            {
                _output.write(reinterpret_cast<const char *>(&value), sizeof((value)));
            }
            void write_string(const std::string &str);
            void write_string(const char *str, uint32_t size);
            void write_string(const uint8_t *str, uint32_t size);
    };

    class RamFileSystemDeserialise
    {
        public:

            RamFileSystemDeserialise(std::istream &input, RamFileSystem *fs);
            void load();

        private:
            
            class Entry
            {
                public:
                    Entry(uint32_t id);

                    uint32_t id() const;

                private:
                    uint32_t _id;
            };
            class File : public Entry
            {
                public:
                    File(uint32_t id, uint32_t size, const uint8_t *data);

                    uint32_t size() const;
                    const uint8_t *data() const;

                private:
                    uint32_t _size;
                    const uint8_t *_data;
            };

            class Folder : public Entry
            {
                public:
                    Folder(uint32_t id);
                    void add_entry(const std::string &name, uint32_t id);

                private:
                    std::map<std::string, uint32_t> _entries;
            };

            std::istream &_input;
            RamFileSystem *_fs;
            std::map<uint32_t, Entry *> _entries;

            void deserialise_entry();
            void deserialise_folder(uint32_t id);
            void deserialise_file(uint32_t id);

            template <class T>
            T read_binary()
            {
                T value;
                _input.read(reinterpret_cast<char *>(&value), sizeof(T));
                return value;
            }

            std::string read_string();
    };
}
