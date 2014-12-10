#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include <iostream>

#include <loss/return_codes.h>

namespace loss
{
    class IMetadata
    {
        public:
            const std::string &owner() const;
            ReturnCode owner(const std::string &new_owner);

            /* Not needed yet {{{
            bool readable() const;
            ReturnCode readable(bool value);
            
            bool writable() const;
            ReturnCode writable(bool value);
            
            bool executable() const;
            ReturnCode executable(bool value);
            }}}*/
    };

    class IFile
    {
        public:
            IMetadata &metadata();
            const IMetadata &metadata() const;

            uint32_t size() const;
    };

    class IFolder
    {
        public:
            IMetadata &metadata();
            const IMetadata &metadata() const;

            IFile &file(const std::string &name);
            IFile &file(uint32_t index);
            uint32_t size() const;
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

            virtual ReturnCode getdir(const std::string &name, IFolder *to_populate) = 0;
    };

}
