#include "OsgVisitors.hpp"
#include <osgDB/ReadFile>

namespace vizkit3d
{
    ::osg::Node * FindNode::find(::osg::Node &node,std::string name)
    {
        FindNode finder(name);
        node.accept(finder);
        return finder.getNode();
    }

    FindNode::FindNode()
        : ::osg::NodeVisitor(::osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    {
    }

    FindNode::FindNode(std::string name) :
        osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
        name(name)
    {
    }

    osg::Node *FindNode::getNode()
    {
        return node.get();
    }

    void FindNode::apply(osg::Node &node)
    {
        if(node.getName() == name)
            this->node = &node;
        else
            traverse(node);
    }
}
