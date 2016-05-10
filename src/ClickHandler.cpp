#include "ClickHandler.hpp"
#include <iostream> //FIXME remove after test

namespace vizkit3d
{

ClickHandler::ClickHandler()
{}

  
ClickHandler::ClickHandler(const osgviz::Object* clickedObject) : 
    clickedObject(clickedObject)
{}

void ClickHandler::setClickedObject(const osgviz::Object* _clickedObject)
{
    clickedObject = _clickedObject;
}


bool ClickHandler::clicked(const int& buttonMask, const osg::Vec2d& cursor,
                           const osg::Vec3d& world, const osg::Vec3d& local,
                           osgviz::Clickable* object, const int modKeyMask,
                           osgviz::WindowInterface* window)
{
    emit objectClicked(buttonMask, cursor, world, local, clickedObject);
    return true;
}

bool ClickHandler::dragged(const int& buttonMask, const osg::Vec2d& cursor,
                           const osg::Vec3d& world, const osg::Vec3d& local,
                           osgviz::Clickable* object, const int modKeyMask,
                           osgviz::WindowInterface* window)
{
    return osgviz::Clickable::dragged(buttonMask, cursor, world, local, object, modKeyMask, window);
}

  
}