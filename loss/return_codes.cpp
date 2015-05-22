#include "return_codes.h"

namespace loss
{
    std::map<ReturnCode, std::string> ReturnCodes::s_code_name = {
        { UNKNOWN_CODE, "unknown_return_code" },

        { SUCCESS,         "success" },
        { NULL_PARAMETER,  "null_parameter" },
        { INTERNAL_ERROR,  "internal_error" },
        { FILE_NOT_FOUND,  "file_not_found" },
        { ALREADY_IN_LIST, "already_in_list" },

        { ENTRY_ALREADY_EXITS,     "entry_already_exits" },
        { WRONG_ENTRY_TYPE,        "wrong_entry_type" },
        { ENTRY_NOT_FOUND,         "entry_not_found" },
        { FOLDER_NOT_EMPTY,        "folder_not_empty" },
        { CANNOT_FIND_FILE_HANDLE, "cannot_find_file_handle" },
        { WRONG_FILE_MODE,         "wrong_file_mode" },
        { FILE_HAS_WRITE_LOCK,     "file_has_write_lock" },
        { HANDLE_LACKING_WRITE,    "handle_lacking_write" },
        { HANDLE_LACKING_READ,     "handle_lacking_read" },
        { VFS_HAS_NO_ROOT,         "vfs_has_no_root" },

        { CANNOT_FIND_PROCESS, "cannot_find_process" },

        { CANNOT_FIND_USER,    "cannot_find_user" },
        { USER_ALREADY_EXISTS, "user_already_exists" },

        { DIFFERENT_KERNEL_SAME_ID, "different_kernel_same_id" }
    };

    std::map<ReturnCode, std::string> ReturnCodes::s_code_desc = {
        { UNKNOWN_CODE, "unknown return code." },

        { SUCCESS,         "success" },
        { NULL_PARAMETER,  "a parameter was null or an empty string." },
        { INTERNAL_ERROR,  "there was an unexpected internal error." },
        { FILE_NOT_FOUND,  "file not found." },
        { ALREADY_IN_LIST, "a parameter has already been added to this list." },

        { ENTRY_ALREADY_EXITS,     "an entry with the given name already exists." },
        { WRONG_ENTRY_TYPE,        "an entry was found but it was not the correct type." },
        { ENTRY_NOT_FOUND,         "no entry with the given name was found." },
        { FOLDER_NOT_EMPTY,        "cannot delete folder while it is not empty." },
        { CANNOT_FIND_FILE_HANDLE, "cannot find file handle with the given process id." },
        { WRONG_FILE_MODE,         "cannot read or write to this file handle based on it's mode." },
        { FILE_HAS_WRITE_LOCK,     "a file can only have one write handle at a time." },
        { HANDLE_LACKING_WRITE,    "attempting to write with a file handle that does not have write permissions." },
        { HANDLE_LACKING_READ,     "attempting to read with a file handle that does not have read permissions." },
        { VFS_HAS_NO_ROOT,         "vfs has no root filesystem set." },
        
        { CANNOT_FIND_PROCESS, "cannot find the process with the given process_id." },

        { CANNOT_FIND_USER, "cannot find user by id." },
        { USER_ALREADY_EXISTS, "a user with the given id already exists." },

        { DIFFERENT_KERNEL_SAME_ID, "a different kernel with the same id has already been registered." }
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
