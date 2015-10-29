#pragma once

#include <stdint.h>

#include "return_codes.h"

namespace loss
{
    class StreamHandle
    {
        public:

            enum OpenMode
            {
                UNKNOWN  = -0x01,
                NONE     = 0x00,
                READ     = 0x01,
                WRITE    = 0x02,
                APPEND   = 0x04
            };

            StreamHandle( uint32_t process_id, OpenMode mode);

            uint32_t process_id() const;
            OpenMode mode() const;
            bool has_write_mode() const;
            bool has_read_mode() const;

            int32_t read_position() const;
            void read_position(int32_t pos);
            void change_read_position(int32_t change);

            int32_t write_position() const;
            void write_position(int32_t pos);
            void change_write_position(int32_t change);

        private:
            uint32_t _process_id;
            int32_t _read_position;
            int32_t _write_position;
            OpenMode _mode;
    };

    inline StreamHandle::OpenMode operator |(StreamHandle::OpenMode lhs, StreamHandle::OpenMode rhs)
    {
        return static_cast<StreamHandle::OpenMode>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }
    inline StreamHandle::OpenMode &operator |=(StreamHandle::OpenMode &lhs, StreamHandle::OpenMode rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }
}
