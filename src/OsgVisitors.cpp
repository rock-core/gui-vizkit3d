#include "OsgVisitors.hpp"
#include <osgDB/ReadFile>

namespace vizkit3d
{
    //
    // FindNode
    //
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


    //
    // SetOpacity
    //
    void SetOpacity::setOpacity(::osg::Node &node, double opacity)
    {
        SetOpacity visitor(opacity);
        node.accept(visitor);
    }

    SetOpacity::SetOpacity()
        : ::osg::NodeVisitor(::osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
    {
    }

    SetOpacity::SetOpacity(double opacity) :
        osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN),
        opacity(opacity)
    {
    }

    void SetOpacity::apply(osg::Node &node){
        traverse(node);
    }

    void SetOpacity::apply(osg::Geode &geode)
    {
        osg::StateSet *state   = NULL;
        unsigned int    vertNum = 0;


        //
        //  We need to iterate through all the drawables check if
        //  the contain any geometry that we will need to process
        //
        unsigned int numGeoms = geode.getNumDrawables();
        for( unsigned int geodeIdx = 0; geodeIdx < numGeoms; geodeIdx++ ) {
            osg::Geometry *curGeom = geode.getDrawable( geodeIdx )->asGeometry();

            //
            // Only process if the drawable is geometry
            //
            if ( curGeom ) {
                osg::Vec4Array *colorArrays = dynamic_cast< osg::Vec4Array *>(curGeom->getColorArray());
                if ( colorArrays ) {
                    for ( unsigned int i = 0; i < colorArrays->size(); i++ ) {
                        osg::Vec4 *color = &colorArrays->operator [](i);
                        color->set( color->r(), color->g(), color->b(), opacity);
                    }
                }
            }
        }
    }
}
