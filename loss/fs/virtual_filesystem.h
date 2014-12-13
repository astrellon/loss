#pragma once

#include "ifilesystem.h"

namespace loss
{
    class VirtualMetadata : public IMetadata
    {
        public:
            virtual const std::string &owner() const;
            virtual ReturnCode owner(const std::string &new_owner);

        private:
            std::string _owner;
    };

    class VirtualFile : public IFile
    {
        public:
            virtual IMetadata &metadata();
            virtual const IMetadata &metadata() const;

            virtual uint32_t size() const;

        private:
            VirtualMetadata _metadata;
            uint32_t _size;
            
    };

    class VirtualFolder : public IFolder
    {
        public:
            virtual IMetadata &metadata();
            virtual const IMetadata &metadata() const;

            virtual IFile *file(const std::string &name) const;
            virtual IFile *file(uint32_t index) const;
            virtual uint32_t num_files() const;
            
            virtual IFolder *folder(const std::string &name) const;
            virtual IFolder *folder(uint32_t index) const;
            virtual uint32_t num_folders() const;

        private:
            VirtualMetadata _metadata;
            std::vector<VirtualFile *> _files;
            std::vector<VirtualFolder *> _folders;
    };

    class VirtualFileSystem : public IFileSystem
    {
        public:
            virtual IOResult read(const std::string &name, uint32_t offset, uint32_t count, uint8_t *buffer);
            virtual IOResult write(const std::string &name, uint32_t offset, uint32_t counter, uint8_t *data);

            virtual ReturnCode getdir(const std::string &name, IFolder *to_populate);
    };
}
