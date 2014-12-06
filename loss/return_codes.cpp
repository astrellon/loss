#include "return_codes.h"

namespace loss
{
    const std::string &getErrorName(loss::ReturnCode code)
    {
        switch (code)
        {
            case SUCCESS: return std::string("success");
            case NULL_PARAMETER: return std::string("null_parameter");
            case INTERNAL_ERROR: return std::string("internal_error");
            case FILE_NOT_FOUND: return std::string("file_not_found");

            case NODE_ALREADY_EXISTS: return std::string("node_already_exists");
            case NODE_NOT_FOUND: return std::string("node_not_found");

            default:
                                 return std::string("unknown_return_code");
        }
    }

    const std::string &getErrorMessage(loss::ReturnCode code)
    {
        switch (code)
        {
            // Base
            case SUCCESS: return std::string("success");

            case NULL_PARAMETER: return std::string("a parameter was null or an empty string.");
            case INTERNAL_ERROR: return std::string("there was an unexpected internal error.");
            case FILE_NOT_FOUND: return std::string("file not found.");

             // INode related
            case NODE_ALREADY_EXISTS: return std::string("a node with the given name already exists.");
            case NODE_NOT_FOUND: return std::string("a node with the given name does not exist.");

            default:
                                 return std::string("unknown return code");
        }
    }
}
