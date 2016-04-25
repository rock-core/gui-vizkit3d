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
        typedef void (*functionType) ();

        enum keyStatusType
        {
            KEY_UP, KEY_DOWN
        };

        struct functionStatusType
        {
            functionStatusType() {keyState = KEY_UP; keyFunction = NULL;}
            functionType keyFunction;
            keyStatusType keyState;
        };

	
    public:
        PickHandler();
        ~PickHandler();

        bool addFunction(int whatKey, functionType newFunction);
        bool addFunction(int whatKey, keyStatusType keyPressStatus, functionType newFunction);

        bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
        void pick(const osgGA::GUIEventAdapter& ea, osgViewer::View* viewer);

    signals:
        void picked(const QVector3D& coord) const;
        /**Is emitted whenenver the user picks a node.
         * @p path The path from the root node to the picked node*/
        void pickedNodePath(const osg::NodePath& path) const;

    protected:
        void setTrackedNode(osgViewer::View* viewer, osg::ref_ptr< osg::Node > node);
        
        /** @param nodePath NodePath to the picked node
         *  @param global  global coordinates of the clicked point*/
        void pickNodePath(const osg::NodePath& nodePath, osg::Vec3 global, 
                          osgViewer::View* viewer) const;

        void wireFrameModeOn(osg::Node *srcNode);
        void wireFrameModeOff(osg::Node *srcNode);

        float _mx,_my;
        bool _usePolytopeIntersector;

        typedef std::map<int, functionStatusType > keyFunctionMap;
        keyFunctionMap keyFuncMap;
        keyFunctionMap keyUPFuncMap;
};
}

#endif
