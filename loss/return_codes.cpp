#include "return_codes.h"

namespace loss
{
    std::map<ReturnCode, std::string> ReturnCodes::s_code_name = {
        { UNKNOWN_CODE, "unknown_return_code" },

        { SUCCESS, "success" },
        { NULL_PARAMETER, "null_parameter" },
        { INTERNAL_ERROR, "internal_error" },
        { FILE_NOT_FOUND, "file_not_found" },

        { NODE_ALREADY_EXISTS, "node_already_exists" },
        { NODE_NOT_FOUND, "node_not_found" }
    };

    std::map<ReturnCode, std::string> ReturnCodes::s_code_desc = {
        { UNKNOWN_CODE, "unknown return code" },

        { SUCCESS, "success" },
        { NULL_PARAMETER, "a parameter was null or an empty string." },
        { INTERNAL_ERROR, "there was an unexpected internal error." },
        { FILE_NOT_FOUND, "file not found" },

        { NODE_ALREADY_EXISTS, "a node with the given name already exists." },
        { NODE_NOT_FOUND, "a node with the given name does not exist." }
    };

    const std::string &ReturnCodes::name(loss::ReturnCode code)
    {
        auto find = s_code_name.find(code);
        if (find == s_code_name.end())
        {
            return s_code_name[UNKNOWN_CODE];
        }
        return find->second;
    }

    const std::string &ReturnCodes::desc(loss::ReturnCode code)
    {
        auto find = s_code_desc.find(code);
        if (find == s_code_desc.end())
        {
            return s_code_desc[UNKNOWN_CODE];
        }
        return find->second;
    }
}
