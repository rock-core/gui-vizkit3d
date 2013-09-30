#ifndef __VIZKIT_GRIDNODE_HPP__
#define __VIZKIT_GRIDNODE_HPP__

#include <osg/Geode>
#include <osg/Geometry>

namespace vizkit
{
    class GridNode
    {
        public:
            /**
              Creates a new sub scene graph for a grid which has the its center at 0/0

              @param rows The number of rows
              @param cols The number of cols 
              @param dx The size of a cell in x direction
              @param dx The size of a cell in y direction
              @param color The color of the grid
              @returns The root node of the created sub graph
              */
            static ::osg::Node* create(int rows,int cols,float dx, float dy,const ::osg::Vec4 &color=::osg::Vec4());
    };
}

#endif
