#pragma once

#include <loss/return_codes.h>
#include <stdint.h>

namespace loss
{
    enum EntryType
    {
        UNKNOWN_ENTRY,
        FILE_ENTRY,
        SYMLINK_ENTRY,
        FOLDER_ENTRY,
        MOUNT_POINT_ENTRY,
        CHARACTER_DEVICE_ENTRY
    };

    class MetadataDef
    {
        public:

            inline MetadataDef(EntryType type) :
                _owner_id(0u),
                _permissions(0x00u),
                _type(type)
            {

            }

            inline uint32_t owner_id() const
            {
                return _owner_id;
            }
            inline void owner_id(uint32_t id)
            {
                _owner_id = id;
            }
            
            enum Permission
            {
                READABLE    = 0x01,
                WRITABLE    = 0x02,
                EXECUTABLE  = 0x04,
                CHAR_DEVICE = 0x08
            };

#define SET_BIT(set, value, mask) \
            set ? value | mask : value & ~mask 

#define GET_BIT(value, mask) \
            (value & mask) > 0u;

            inline bool readable() const
            {
                return GET_BIT(_permissions, READABLE);
            }
            inline void readable(bool set)
            {
                _permissions = SET_BIT(set, _permissions, READABLE);
            }

            inline bool writable() const
            {
                return GET_BIT(_permissions, WRITABLE);
            }
            inline void writable(bool set)
            {
                _permissions = SET_BIT(set, _permissions, WRITABLE);
            }

            inline bool executable() const
            {
                return GET_BIT(_permissions, EXECUTABLE);
            }
            inline void executable(bool set)
            {
                _permissions = SET_BIT(set, _permissions, EXECUTABLE);
            }

            inline bool character_device() const
            {
                return GET_BIT(_permissions, CHAR_DEVICE);
            }
            inline void character_device(bool set)
            {
                _permissions = SET_BIT(set, _permissions, CHAR_DEVICE);
            }

            inline uint32_t permissions() const
            {
                return _permissions;
            }
            inline void permissions(uint32_t value)
            {
                _permissions = value;
            }

            inline EntryType type() const
            {
                return _type;
            }

        private:
            uint32_t _owner_id;
            uint32_t _permissions;
            EntryType _type;
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
                _fs(fs),
                _metadata(UNKNOWN_ENTRY)
            {

            }
            FindEntryResult(uint32_t id, ReturnCode status, IFileSystem *fs, const MetadataDef &metadata) :
                _id(id),
                _status(status),
                _fs(fs),
                _metadata(metadata)
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
            inline const MetadataDef &metadata() const
            {
                return _metadata;
            }

        private:
            uint32_t _id;
            ReturnCode _status;
            IFileSystem *_fs;
            MetadataDef _metadata;
    };
}
