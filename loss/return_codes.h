#pragma once

#include <string>

namespace loss
{
    enum ReturnCode
    {
		// General
		SUCCESS =					0x0001,
		NULL_PARAMETER =			0x0000,
		INTERNAL_ERROR =			-0x0001,
		FILE_NOT_FOUND =			-0x0002,

        // INode related
        NODE_ALREADY_EXISTS =       -0x0003,
        NODE_NOT_FOUND =            -0x0004
    };

    const std::string &getErrorName(loss::ReturnCode code);
    const std::string &getErrorMessage(loss::ReturnCode code);
}
