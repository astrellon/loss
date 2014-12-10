#pragma once

#include <string>
#include <stdint.h>
#include <map>
#include <vector>

#include <loss/return_codes.h>

namespace loss
{
    class IFileSystem;

    class INode
    {
        public:

            INode(IFileSystem *fs);
            ~INode();
            
            uint32_t symlink_count() const;
            void symlink_count(uint32_t value);
            void inc_symlink();
            void dec_symlink();

            typedef std::map<std::string, INode *> NodeMap;
            ReturnCode add_node(const std::string &name, INode *node);
            ReturnCode remove_node(const std::string &name);
            INode *find_node(const std::string &name) const;
            uint32_t num_nodes() const;

            NodeMap::const_iterator begin_nodes() const;
            NodeMap::const_iterator end_nodes() const;
            
            typedef std::vector<uint8_t> DataList;
            uint32_t data_size() const;
            DataList::const_iterator begin_data() const;
            DataList::const_iterator end_data() const;
            DataList &data();

            IFileSystem *filesystem() const;

        private:
            uint32_t _symlink_count;
            NodeMap _nodes;
            DataList _data;
            IFileSystem *_fs;

            NodeMap::const_iterator find_node_iter(const std::string &name) const;
            
    };
}
