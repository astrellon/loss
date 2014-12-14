#pragma once

#include <string>
#include <vector>
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

    class FileDef
    {
        public:
            FileDef();

            MetadataDef &metadata();
            const MetadataDef &metadata() const;

            uint32_t size() const;
            void size(uint32_t size);

        private:
            MetadataDef _metadata;
            uint32_t _size;
    };

    class FolderDef
    {
        public:
            MetadataDef &metadata();
            const MetadataDef &metadata() const;

            ReturnCode add_file(const std::string &name, const FileDef &file);
            ReturnCode find_file(const std::string &name, FileDef && file) const;
            ReturnCode add_folder(const std::string &name, const FolderDef &folder);
            ReturnCode find_folder(const std::string &name, FolderDef && folder) const;
            bool has_file_folder(const std::string &name);

            typedef std::map<std::string, FileDef &> FileMap;
            typedef std::map<std::string, FolderDef &> FolderMap;

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
            IOResult(uint32_t bytes, ReturnCode status)
            {
                _bytes = bytes;
                _status = status;
            }

            inline uint32_t bytes() const
            {
                return _bytes;
            }
            ReturnCode status() const
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
            virtual IOResult write(const std::string &name, uint32_t offset, uint32_t counter, uint8_t *data) = 0;

            virtual ReturnCode getdir(const std::string &name, FolderDef *to_populate) = 0;
    };

}
