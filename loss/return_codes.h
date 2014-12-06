#pragma once

#include <string>
#include <map>

namespace loss
{
    enum ReturnCode
    {
        UNKNOWN_CODE =              -0xFFFF,

		// General
		SUCCESS =					0x0001,
		NULL_PARAMETER =			0x0000,
		INTERNAL_ERROR =			-0x0001,
		FILE_NOT_FOUND =			-0x0002,

        // INode related
        NODE_ALREADY_EXISTS =       -0x0003,
        NODE_NOT_FOUND =            -0x0004
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
