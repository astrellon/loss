#pragma once

#include <vector>
#include <string>

namespace loss
{
    class Path
    {
        public:

            typedef std::vector<std::string> DirList;
            Path();
            Path(const std::string &path);
            Path(const Path &path);

            void set_dir(const std::string &path);
            void push_dir(const std::string &path);
            void push_dir(const Path &path);
            std::string pop_dir();

            const DirList &dirs() const;

            void filename_to_dir();
            void dir_to_filename();

            const std::string &filename() const;
            void filename(const std::string &filename);

            bool is_root() const;
            bool absolute() const;
            void absolute(bool value);

            std::string to_string() const;

            Path subpath(uint32_t start, uint32_t end = -1) const;

        private:
            DirList _dirs;
            std::string _filename;
            bool _absolute;

            DirList split_path(const std::string &path);
    };
}
