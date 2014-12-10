#include "inode.h"

#include "ifilesystem.h"

namespace loss
{
    INode::INode(IFileSystem *fs) : 
        _symlink_count(0),
        _fs(fs)
    {

    }
    INode::~INode()
    {

    }

    IFileSystem *INode::filesystem() const
    {
        return _fs;
    }

    uint32_t INode::symlink_count() const
    {
        return _symlink_count;
    }
    void INode::symlink_count(uint32_t value)
    {
        _symlink_count = value; 
    }
    void INode::inc_symlink()
    {
        ++_symlink_count;
    }
    void INode::dec_symlink()
    {
        --_symlink_count;
    }

    ReturnCode INode::add_node(const std::string &name, INode *node)
    {
        auto find = find_node_iter(name);
        if (find != _nodes.end())
        {
            return NODE_ALREADY_EXISTS;
        }

        _nodes[name] = node;
        node->inc_symlink();
        return SUCCESS;
    }
    ReturnCode INode::remove_node(const std::string &name)
    {
        auto find = find_node_iter(name);
        if (find == _nodes.end())
        {
            return NODE_NOT_FOUND;
        }

        _nodes.erase(find);
        find->second->dec_symlink();
        return SUCCESS;
    }

    INode *INode::find_node(const std::string &name) const
    {
        auto find = find_node_iter(name);
        if (find == _nodes.end())
        {
            return nullptr;
        }

        return find->second;
    }

    uint32_t INode::num_nodes() const
    {
        return static_cast<uint32_t>(_nodes.size());
    }

    INode::NodeMap::const_iterator INode::begin_nodes() const
    {
        return _nodes.begin();
    }
    INode::NodeMap::const_iterator INode::end_nodes() const
    {
        return _nodes.end();
    }

    uint32_t INode::data_size() const
    {
        return static_cast<uint32_t>(_data.size());
    }

    INode::DataList::const_iterator INode::begin_data() const
    {
        return _data.begin();
    }
    INode::DataList::const_iterator INode::end_data() const
    {
        return _data.end();
    }
    INode::DataList &INode::data()
    {
        return _data;
    }

    INode::NodeMap::const_iterator INode::find_node_iter(const std::string &name) const
    {
        for (auto iter = _nodes.begin(); iter != _nodes.end(); ++iter)
        {
            if (iter->first == name)
            {
                return iter;
            }
        }
        return _nodes.end();
    }
}
