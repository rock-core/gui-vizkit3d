#ifndef __VIZKIT_FINDNODE_HPP
#define __VIZKIT_FINDNODE_HPP

#include <osg/NodeVisitor>
#include <osg/Geometry>
#include <string>

namespace vizkit3d
{
    class FindNode : public ::osg::NodeVisitor
    {
        public:
            static ::osg::Node *find(::osg::Node &node,std::string name);

            FindNode();
            FindNode(std::string name);

            ::osg::Node *getNode();
            void apply(::osg::Node &node);
        private:
            ::osg::ref_ptr< ::osg::Node> node;
            std::string name;
    };


    class SetOpacity : public ::osg::NodeVisitor
    {
        public:
            static void setOpacity(::osg::Node &node, double opacity);

            SetOpacity();
            SetOpacity(double opacity);

            void apply(osg::Node &node );
            void apply(::osg::Geode &geode);
        private:
            ::osg::ref_ptr< ::osg::Node> node;
            double opacity;
    };
}
#endif
