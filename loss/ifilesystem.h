#pragma once

#include <string>
#include <stdint.h>

namespace loss
{
    class IFileSystem
    {
        public:
            virtual INode *node(const std::string &path) const;

            virtual INode *root_node() const;
    };
}
