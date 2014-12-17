#include "path.h"

#include <sstream>

namespace loss
{
    Path::Path() :
        _absolute(false)
    {

    }
    Path::Path(const std::string &path) :
        _absolute(false)
    {
        set_dir(path);
    }
    Path::Path(const Path &path) :
        _filename(path._filename),
        _absolute(path._absolute)
    {
        for (auto part : path._dirs)
        {
            push_dir(part);
        }
    }

    void Path::set_dir(const std::string &path)
    {
        _absolute = path.size() == 0 || path[0] == '/';
        push_dir(path);
    }
    void Path::push_dir(const std::string &path)
    {
        if (path.find('/') != std::string::npos)
        {
            auto split = split_path(path);
            for (auto part : split)
            {
                _dirs.push_back(part);
            }
        }
        else
        {
            _dirs.push_back(path);
        }

        if (_dirs.back().find('.') != std::string::npos)
        {
            dir_to_filename();
        }
    }
    void Path::push_dir(const Path &path)
    {
        for (auto part : path._dirs)
        {
            _dirs.push_back(part);
        }
    }
    std::string Path::pop_dir()
    {
        if (_dirs.size() > 0)
        {
            auto temp = _dirs.back();
            _dirs.pop_back();
            return temp;
        }
        return std::string();
    }

    const std::vector<std::string> &Path::dirs() const
    {
        return _dirs;
    }


    void Path::filename_to_dir()
    {
        if (_filename.size() > 0)
        {
            _dirs.push_back(_filename);
            _filename.clear();
        }
    }
    void Path::dir_to_filename()
    {
        if (_filename.size() == 0 && _dirs.size() > 0)
        {
            _filename = _dirs.back();
            _dirs.pop_back();
        }
    }

    const std::string &Path::filename() const
    {
        return _filename;
    }
    void Path::filename(const std::string &filename)
    {
        _filename = filename;
    }

    bool Path::is_root() const
    {
        return _absolute && _dirs.size() == 0;
    }
    bool Path::absolute() const
    {
        return _absolute;
    }
    void Path::absolute(bool value)
    {
        _absolute = value;
    }

    std::string Path::to_string() const
    {
        std::stringstream ss;
        if (!_absolute)
        {
            ss << '.';
        }

        for (auto part : _dirs)
        {
            ss << '/' << part;
        }

        if (_filename.size() > 0)
        {
            ss << '/' << _filename;
        }

        return ss.str();
    }

    Path Path::subpath(uint32_t start, uint32_t end) const
    {
        Path result;
        for (uint32_t i = start; i < end && i < _dirs.size(); i++)
        {
            result._dirs.push_back(_dirs[i]);
        }
        result._filename = _filename;
        return result;
    }

    Path::DirList Path::split_path(const std::string &path)
    {
        size_t start_index = 0u;
        size_t end_index = path.find('/');

        DirList result;
        while (start_index != std::string::npos)
        {
            std::string sub = path.substr(start_index, end_index - start_index);
            if (sub.size() != 0)
            {
                result.push_back(sub);
            }
            if (end_index == std::string::npos)
            {
                break;
            }

            start_index = end_index + 1;
            end_index = path.find('/', start_index + 1);
        }
        return result;
    }
}
