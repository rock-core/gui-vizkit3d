#ifndef _VIZKIT_3D_CLICK_HANDLER_HPP_
#define _VIZKIT_3D_CLICK_HANDLER_HPP_

#include <osgViz/interfaces/Clickable.h>
#include <osgViz/plugins/Object.h>

namespace vizkit3d
{

class ClickHandler : public osgviz::Clickable
{
public:
    ClickHandler(const osgviz::Object* clickedObject);
    ClickHandler();
    
    void setClickedObject(const osgviz::Object* clickedObject);
    
    virtual bool clicked(const int& buttonMask, const osg::Vec2d& cursor,
                        const osg::Vec3d& world, const osg::Vec3d& local,
                        Clickable* object, osgviz::WindowInterface* window = 0);

    virtual bool dragged(const int& buttonMask, const osg::Vec2d& cursor,
                        const osg::Vec3d& world, const osg::Vec3d& local, 
                        Clickable* object, osgviz::WindowInterface* window = 0);
private:
    const osgviz::Object* clickedObject;
};

}

#endif