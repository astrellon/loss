#pragma once

#include <loss/return_codes.h>
#include <stdint.h>

namespace loss
{
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

    class IFileSystem;

    class FindFolderResult
    {
        public:
            FindFolderResult(uint32_t id, ReturnCode status, IFileSystem *fs) :
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
