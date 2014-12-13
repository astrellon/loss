#include "ifilesystem.h"

namespace loss
{
    MetadataDef::MetadataDef() :
        _owner("No owner")
    {

    }
    const std::string &MetadataDef::owner() const
    {
        return _owner;
    }
    ReturnCode MetadataDef::owner(const std::string &new_owner)
    {
        if (new_owner.empty())
        {
            return NULL_PARAMETER;
        }
        _owner = new_owner;
        return SUCCESS;
    }

    FileDef::FileDef() :
        _size(0)
    {

    }
    MetadataDef &FileDef::metadata()
    {
        return _metadata;
    }
    const MetadataDef &FileDef::metadata() const
    {
        return _metadata;
    }
    uint32_t FileDef::size() const
    {
        return _size;
    }
    void FileDef::size(uint32_t size)
    {
        _size = size;
    }

    MetadataDef &FolderDef::metadata()
    {
        return _metadata;
    }
    const MetadataDef &FolderDef::metadata() const
    {
        return _metadata;
    }

    ReturnCode FolderDef::add_file(const std::string &name, const FileDef &file)
    {
        return SUCCESS;
    }

    
}
