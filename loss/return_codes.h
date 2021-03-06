#pragma once

#include <string>
#include <map>

namespace loss
{
    enum ReturnCode
    {
        UNKNOWN_CODE =              -0xFFFF,

        // General
        SUCCESS =                    0x0001,
        ALREADY_IN_LIST =            0x0002,

        NULL_PARAMETER =             0x0000,
        INTERNAL_ERROR =            -0x0001,
        FILE_NOT_FOUND =            -0x0002,
        OUT_OF_BOUNDS =             -0x0003,
        INVALID_NUMBER =            -0x0004,

        // FileSystem related
        ENTRY_ALREADY_EXITS =       -0x0010,
        WRONG_ENTRY_TYPE =          -0x0011,
        ENTRY_NOT_FOUND =           -0x0012,
        FOLDER_NOT_EMPTY =          -0x0013,
        CANNOT_FIND_FILE_HANDLE =   -0x0014,
        WRONG_FILE_MODE =           -0x0015,
        FILE_HAS_WRITE_LOCK =       -0x0016,
        HANDLE_LACKING_WRITE =      -0x0017,
        HANDLE_LACKING_READ =       -0x0018,
        VFS_HAS_NO_ROOT =           -0x0019,
        MUST_BE_SAME_FS =           -0x001A,

        // Process related
        CANNOT_FIND_PROCESS =       -0x0020,
            
        // User related
        CANNOT_FIND_USER =          -0x0040,
        USER_ALREADY_EXISTS =       -0x0041,

        // Kernel related
        DIFFERENT_KERNEL_SAME_ID =  -0x0100,

    };

    class ReturnCodes
    {
        public:
            static const std::string &name(ReturnCode code);
            static const std::string &desc(ReturnCode code);

        private:
            static std::map<ReturnCode, std::string> s_code_name;
            static std::map<ReturnCode, std::string> s_code_desc;
    };

}
