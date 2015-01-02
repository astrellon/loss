#pragma once

#include <loss/return_codes.h>
#include <stdint.h>

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

    class IOResult
    {
        private:
            uint32_t _bytes;
            ReturnCode _status;

        public:
            inline IOResult(uint32_t bytes, ReturnCode status) :
                _bytes(bytes),
                _status(status)
            {

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

    class CreateEntryResult
    {
        private:
            uint32_t _id;
            ReturnCode _status;

        public:
            inline CreateEntryResult(uint32_t id, ReturnCode status) :
                _id(id),
                _status(status)
            {

            }

            inline uint32_t id() const
            {
                return _id;
            }
            inline ReturnCode status() const
            {
                return _status;
            }
    };

    class IFileSystem;

    class FindEntryResult
    {
        public:
            FindEntryResult(uint32_t id, ReturnCode status, IFileSystem *fs) :
                _id(id),
                _status(status),
                _fs(fs)
            {

            }

            inline uint32_t id() const
            {
                return _id;
            }
            inline ReturnCode status() const
            {
                return _status;
            }
            inline IFileSystem *fs() const
            {
                return _fs;
            }

        private:
            uint32_t _id;
            ReturnCode _status;
            IFileSystem *_fs;
    };
}
