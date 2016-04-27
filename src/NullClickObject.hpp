#ifndef _VIZKIT_3D_NULL_CLICK_HANDLER_HPP_
#define _VIZKIT_3D_NULL_CLICK_HANDLER_HPP_

#include <osgViz/plugins/Object.h>

/**An osgviz object that does nothing but stop events from propagating up the chain */
class NullClickObject : public osgviz::Object
{
virtual bool clicked(const int& buttonMask, const osg::Vec2d& cursor,
                     const osg::Vec3d& world, const osg::Vec3d& local,
                     Clickable* object, osgviz::WindowInterface* window = 0)
{
    return true;
}
virtual bool dragged(const int& buttonMask, const osg::Vec2d& cursor,
                     const osg::Vec3d& world, const osg::Vec3d& local,
                     Clickable* object, osgviz::WindowInterface* window = 0)
{
    return true;
}
  
};


#endif
