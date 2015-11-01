#pragma once

#include <stdint.h>

#include "return_codes.h"
#include "io_common.h"

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

            virtual void close() = 0;

            virtual IOResult read(uint32_t count, uint8_t *buffer) = 0;
            IOResult read_till_character(char terminator, uint32_t max_count, uint8_t *buffer);
            IOResult read_number(double &num);

            virtual IOResult write(uint32_t count, const uint8_t *data) = 0;
            virtual ReturnCode size(uint32_t &size) = 0;
            virtual bool at_eof() = 0;

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
