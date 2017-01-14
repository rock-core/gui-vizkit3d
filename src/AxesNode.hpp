#ifndef __VIZKIT_AXESNODE_HPP__
#define __VIZKIT_AXESNODE_HPP__

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Referenced>
#include <QtCore>
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
            static ::osg::Node* create(float scale=1.0,bool blabels=false);

            /**
             * Sets the position of the center of the axes
             */
            static void setPosition(::osg::Node* axes,const ::osg::Vec3 &position);

            /**
             * Displays the labels for the axes
             */
            static void displayLabels(::osg::Node* axes,bool blabels);

            /**
             * Returns true if the labels are turened on
             */
            static bool hasLabels(::osg::Node* axes);

            /**
             * Returns the switch for the axes labels 
             */
            static osg::Switch* getLabelSwitch(::osg::Node* axes);
    };
}

#endif
