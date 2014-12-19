#pragma once

#include <string>
#include <stdint.h>
#include <iostream>

#include <loss/return_codes.h>

namespace loss
{
    class MetadataDef
    {
        public:
            MetadataDef();

            const std::string &owner() const;
            ReturnCode owner(const std::string &new_owner);

            /* Not needed yet {{{
            bool readable() const;
            bool writable() const;
            bool executable() const;
            }}}*/
        private:
            std::string _owner;
    };

    class IFileSystem;

    class IEntry
    {
        public:
            IEntry(IFileSystem *fs);

            virtual MetadataDef &metadata() = 0;
            virtual const MetadataDef &metadata() const = 0;

            IFileSystem *filesystem() const;
        private:
            IFileSystem *_fs;
    };

    class FileEntry : public IEntry
    {
        public:
            FileEntry(IFileSystem *fs);
            
            virtual MetadataDef &metadata();
            virtual const MetadataDef &metadata() const;

            uint32_t size() const;
            void size(uint32_t size);

        private:
            MetadataDef _metadata;
            uint32_t _size;
    };

    class FolderEntry : public IEntry
    {
        public:
            FolderEntry(IFileSystem *fs);

            virtual MetadataDef &metadata();
            virtual const MetadataDef &metadata() const;

            ReturnCode add_file(const std::string &name, FileEntry *file);
            ReturnCode find_file(const std::string &name, FileEntry *file) const;
            ReturnCode add_folder(const std::string &name, FolderEntry *folder);
            ReturnCode find_folder(const std::string &name, FolderEntry *folder) const;
            
            ReturnCode find_entry(const std::string &name, IEntry *entry) const;
            bool has_entry(const std::string &name) const;

            typedef std::map<std::string, FileEntry *> FileMap;
            typedef std::map<std::string, FolderEntry *> FolderMap;

            FileMap::const_iterator begin_files() const;
            FileMap::const_iterator end_files() const;
            uint32_t num_files() const;

            FolderMap::const_iterator begin_folders() const;
            FolderMap::const_iterator end_folders() const;
            uint32_t num_folders() const;

        private:
            MetadataDef _metadata;
            FileMap _files;
            FolderMap _folders;
    };

    class IOResult
    {
        private:
            uint32_t _bytes;
            ReturnCode _status;

        public:
            inline IOResult(uint32_t bytes, ReturnCode status)
            {
                _bytes = bytes;
                _status = status;
            }

            inline uint32_t bytes() const
            {
                return _bytes;
            }
            inline ReturnCode status() const
            {
                return _status;
            }
    };

    class IFileSystem
    {
        public:
            //virtual ReturnCode open(const std::string &name) = 0;
            //virtual ReturnCode symlink(const std::string &link_filename, const std::string &destination) = 0;
            
            // Change to a stream version at some point.
            virtual IOResult read(const std::string &name, uint32_t offset, uint32_t count, uint8_t *buffer) = 0;
            virtual IOResult write(const std::string &name, uint32_t offset, uint32_t count, const uint8_t *data) = 0;
            virtual ReturnCode create_folder(const std::string &name) = 0;

            // Helpers
            IOResult read_stream(const std::string &name, uint32_t offset, uint32_t count, std::ostream &ss);
            IOResult write_string(const std::string &name, uint32_t offset, const std::string &data);

            virtual ReturnCode getdir(const std::string &name, FolderEntry *to_populate) = 0;
    };

}
