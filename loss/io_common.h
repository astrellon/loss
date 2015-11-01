#pragma once

#include <stdint.h>
#include "return_codes.h"

namespace loss
{
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
}
