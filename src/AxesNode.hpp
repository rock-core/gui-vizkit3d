#ifndef __VIZKIT_AXESNODE_HPP__
#define __VIZKIT_AXESNODE_HPP__

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Referenced>
#include <QVector3D>
#include <QColor>
#include <QGenericMatrix>
#include <string>

namespace vizkit3d
{
    class AxesNode
    {
        public:
            /**
              Creates a new sub scene graph for coordinate axes

              @returns The root node of the created sub graph
              */
            static ::osg::Node* create(float scale=1.0);

            /**
             * Sets the position of the center of the axes
             */
            static void setPosition(::osg::Node* axes,const ::osg::Vec3 &position);
    };
}

#endif
