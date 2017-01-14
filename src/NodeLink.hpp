#ifndef __VIZKIT_LINKNODE_HPP__
#define __VIZKIT_LINKNODE_HPP__

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Referenced>
#include <QtCore>
#include <string>

namespace vizkit
{
    class NodeLink
    {
        public:
            class NodeCallback : public osg::NodeCallback
            {
                public:
                    NodeCallback(osg::Node *node1,osg::Node *node2);
                    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

                private:
                    osg::observer_ptr<osg::Node> node1;
                    osg::observer_ptr<osg::Node> node2;
            };
            static osg::Node* create(osg::Node *node1, osg::Node *node2,const osg::Vec4 &color=osg::Vec4());
    };
}

#endif
