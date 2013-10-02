#ifndef __VIZKIT_PICKHANDLER_HPP__
#define __VIZKIT_PICKHANDLER_HPP__

#include <QObject>
#include <QVector3D>
#include <osgViewer/Viewer>
#include <iostream>

#include "Vizkit3DPlugin.hpp"

namespace vizkit3d
{
class PickedCallback : public osg::Referenced
{
public:
    virtual void picked() = 0;
};

class PickedUserData : public osg::Referenced
{
public:
    PickedUserData(VizPluginBase* plugin){ this->plugin = plugin; }
    VizPluginBase* getPlugin() { return plugin; }
private:
    VizPluginBase* plugin;
};

// class to handle events with a pick
class PickHandler : public QObject, public osgGA::GUIEventHandler
{
    Q_OBJECT
	
public: 
    PickHandler();
    ~PickHandler();

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void pick(const osgGA::GUIEventAdapter& ea, osgViewer::View* viewer);

signals:
    void picked(const QVector3D& coord);

protected:
    void setTrackedNode(osgViewer::View* viewer, osg::ref_ptr< osg::Node > node);

    float _mx,_my;
    bool _usePolytopeIntersector;
    bool _useWindowCoordinates;
};
}

#endif
