#include <osg/Timer>
#include <osg/io_utils>
#include <osg/observer_ptr>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>
#include <osgUtil/LineSegmentIntersector>

#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/NodeTrackerManipulator>

#include "PickHandler.hpp"

using namespace vizkit3d;

PickHandler::PickHandler():
    _mx(0.0),_my(0.0),
    _usePolytopeIntersector(false),
    _useWindowCoordinates(false) {}

PickHandler::~PickHandler() {}

bool PickHandler::addFunction(int whatKey, functionType newFunction)
{
   if ( keyFuncMap.end() != keyFuncMap.find( whatKey ))
   {
      std::cout << "duplicate key '" << whatKey << "' ignored." << std::endl;
      return false;
   }
   else
   {
      keyFuncMap[whatKey].keyFunction = newFunction;
      return true;
   }
}

bool PickHandler::addFunction (int whatKey, keyStatusType keyPressStatus, functionType newFunction)
{
   if (keyPressStatus == KEY_DOWN)
   {
      return addFunction(whatKey,newFunction);
   }
   else
   {
      if ( keyUPFuncMap.end() != keyUPFuncMap.find( whatKey )) 
      {
         std::cout << "duplicate key '" << whatKey << "' ignored." << std::endl;
         return false;
      }
      else
      {
         keyUPFuncMap[whatKey].keyFunction = newFunction;
         return true;
      }
   } // KEY_UP
}

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (!viewer) return false;

    bool newKeyDownEvent = false;

    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::KEYDOWN):
            {
                if (ea.getKey()=='w')
		{
                    this->wireFrameModeOn(viewer->getSceneData());
		}
                else if (ea.getKey()=='n')
		{
                    this->wireFrameModeOff(viewer->getSceneData());
		}
                else
                {
                    keyFunctionMap::iterator itr = keyFuncMap.find(ea.getKey());
                    if (itr != keyFuncMap.end())
                    {
                        if ( (*itr).second.keyState == KEY_UP )
                        {
                            (*itr).second.keyState = KEY_DOWN;
                            newKeyDownEvent = true;
                        }
                        if (newKeyDownEvent)
                        {
                            (*itr).second.keyFunction();
                            newKeyDownEvent = false;
                        }
                        return true;
                    }
                }
                return false;
            }
	case(osgGA::GUIEventAdapter::KEYUP):
	    {
		if (ea.getKey()=='p')
		{
		    _usePolytopeIntersector = !_usePolytopeIntersector;
		    if (_usePolytopeIntersector)
		    {
			osg::notify(osg::NOTICE)<<"Using PolytopeIntersector"<<std::endl;
		    } else {
			osg::notify(osg::NOTICE)<<"Using LineSegmentIntersector"<<std::endl;
		    }
		}
		else if (ea.getKey()=='c')
		{
		    _useWindowCoordinates = !_useWindowCoordinates;
		    if (_useWindowCoordinates)
		    {
			osg::notify(osg::NOTICE)<<"Using window coordinates for picking"<<std::endl;
		    } else {
			osg::notify(osg::NOTICE)<<"Using projection coordiates for picking"<<std::endl;
		    }
		}
                else
                {
                    keyFunctionMap::iterator itr = keyFuncMap.find(ea.getKey());
                    if (itr != keyFuncMap.end() )
                    {
                        (*itr).second.keyState = KEY_UP;
                    }

                    itr = keyUPFuncMap.find(ea.getKey());
                    if (itr != keyUPFuncMap.end())
                    {
                        (*itr).second.keyFunction();
                        return true;
                    }
                }

		return false;
	    }
	case(osgGA::GUIEventAdapter::PUSH):
	case(osgGA::GUIEventAdapter::MOVE):
	    {
		_mx = ea.getX();
		_my = ea.getY();
		return false;
	    }
	case(osgGA::GUIEventAdapter::RELEASE):
	    {
		if (_mx == ea.getX() && _my == ea.getY())
		{
		    // only do a pick if the mouse hasn't moved
		    pick(ea,viewer);
		}
		return false;
            }

	default:
	    return false;
    }
}

void PickHandler::pick(const osgGA::GUIEventAdapter& ea, osgViewer::View* viewer)
{
    osg::Node* scene = viewer->getSceneData();
    if (!scene) return;

    osg::notify(osg::NOTICE)<<std::endl;

    osg::Node* node = 0;

    if (_usePolytopeIntersector)
    {
	osgUtil::PolytopeIntersector* picker;
	if (_useWindowCoordinates)
	{
	    // use window coordinates
	    // remap the mouse x,y into viewport coordinates.
	    osg::Viewport* viewport = viewer->getCamera()->getViewport();
	    double mx = viewport->x() + (int)((double )viewport->width()*(ea.getXnormalized()*0.5+0.5));
	    double my = viewport->y() + (int)((double )viewport->height()*(ea.getYnormalized()*0.5+0.5));

	    // half width, height.
	    double w = 5.0f;
	    double h = 5.0f;
	    picker = new osgUtil::PolytopeIntersector( osgUtil::Intersector::WINDOW, mx-w, my-h, mx+w, my+h );
	} else {
	    double mx = ea.getXnormalized();
	    double my = ea.getYnormalized();
	    double w = 0.05;
	    double h = 0.05;
	    picker = new osgUtil::PolytopeIntersector( osgUtil::Intersector::PROJECTION, mx-w, my-h, mx+w, my+h );
	}
	osgUtil::IntersectionVisitor iv(picker);

	viewer->getCamera()->accept(iv);

	if (picker->containsIntersections())
	{
	    for(std::multiset<osgUtil::PolytopeIntersector::Intersection>::iterator it = picker->getIntersections().begin();
		    it != picker->getIntersections().end(); it++)
	    {
		const osgUtil::PolytopeIntersector::Intersection &intersection = *it;

		/*
		osg::notify(osg::NOTICE)<<"Picked "<<intersection.localIntersectionPoint<<std::endl
		    <<"  Distance to ref. plane "<<intersection.distance
		    <<", max. dist "<<intersection.maxDistance
		    <<", primitive index "<<intersection.primitiveIndex
		    <<", numIntersectionPoints "
		    <<intersection.numIntersectionPoints
		    <<std::endl;
		    */
		const osg::NodePath& nodePath = intersection.nodePath;
		node = (nodePath.size()>=1)?nodePath[nodePath.size()-1]:0;

		//osg::Matrixd l2w = osg::computeLocalToWorld( nodePath );
		//osg::Vec3 global = *intersection.matrix.get() * intersection.localIntersectionPoint;
		osg::Vec3 global = intersection.localIntersectionPoint * *intersection.matrix.get();

		QVector3D globalPoint( global.x(), global.y(), global.z());
		emit picked(globalPoint);
	    }
	}
    }
    else
    {
	osgUtil::LineSegmentIntersector* picker;
	if (!_useWindowCoordinates)
	{
	    // use non dimensional coordinates - in projection/clip space
	    picker = new osgUtil::LineSegmentIntersector( osgUtil::Intersector::PROJECTION, ea.getXnormalized(),ea.getYnormalized() );
	} else {
	    // use window coordinates
	    // remap the mouse x,y into viewport coordinates.
	    osg::Viewport* viewport = viewer->getCamera()->getViewport();
	    float mx = viewport->x() + (int)((float)viewport->width()*(ea.getXnormalized()*0.5f+0.5f));
	    float my = viewport->y() + (int)((float)viewport->height()*(ea.getYnormalized()*0.5f+0.5f));
	    picker = new osgUtil::LineSegmentIntersector( osgUtil::Intersector::WINDOW, mx, my );
	}
	osgUtil::IntersectionVisitor iv(picker);

	viewer->getCamera()->accept(iv);

	if (picker->containsIntersections())
	{
	    osgUtil::LineSegmentIntersector::Intersection intersection = picker->getFirstIntersection();

	    osg::NodePath& nodePath = intersection.nodePath;
	    node = (nodePath.size()>=1)?nodePath[nodePath.size()-1]:0;

	    // see if the object has a user object which is derived from pickcallback
	    PickedCallback *pc = dynamic_cast<PickedCallback*>(node->getUserData());
	    if( pc )
		pc->picked();

	    for(int i = nodePath.size()-1; i >= 0; i--)
	    {
	        osg::Node *node = nodePath[i];
	        osg::Referenced *user_data = node->getUserData();
	        if (!user_data)
	            continue;
	        PickedUserData *plugin_data = dynamic_cast<PickedUserData*>(user_data);
	        if(!plugin_data)
	            continue;

                // Transform OSG viewport coordinates to QWidget coordinates (invert y axis)
                float wy = (float)viewer->getCamera()->getViewport()->height() - _my;
                float wx = _mx;
                plugin_data->getPlugin()->click(wx, wy);
                break;
	    }
            // setTrackedNode(viewer, node);
	}
    }
}

void PickHandler::setTrackedNode(osgViewer::View* viewer, osg::ref_ptr< osg::Node > node)
{
    osgGA::KeySwitchMatrixManipulator *keyswitchManipulator = 
    dynamic_cast<osgGA::KeySwitchMatrixManipulator*>(viewer->getCameraManipulator());

    if( !keyswitchManipulator )
    return;

    osgGA::NodeTrackerManipulator *tracker = 
    dynamic_cast<osgGA::NodeTrackerManipulator*>(keyswitchManipulator->getMatrixManipulatorWithIndex( 2 ));
    if( tracker )
    {
    tracker->setTrackerMode( osgGA::NodeTrackerManipulator::NODE_CENTER );
    tracker->setTrackNode( node );
    }
}

void PickHandler::wireFrameModeOn(osg::Node *srcNode)
{
    //Quick sanity check , we need a node
    if( srcNode == NULL )
        return;

    //Grab the state set of the node, this  will a StateSet if one does not exist
    osg::StateSet *state = srcNode->getOrCreateStateSet();

    //We need to retrieve the Polygon mode of the  state set, and create one if does not have one
    osg::PolygonMode *polyModeObj;

    polyModeObj = dynamic_cast< osg::PolygonMode* >( state->getAttribute( osg::StateAttribute::POLYGONMODE ));

    if ( !polyModeObj )
    {
        polyModeObj = new osg::PolygonMode;
        state->setAttribute( polyModeObj );
    }

    //Now we can set the state to WIREFRAME
    polyModeObj->setMode(  osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );

}

void PickHandler::wireFrameModeOff(osg::Node *srcNode)
{
    //Quick sanity check , we need a node
    if( srcNode == NULL )
        return;

    //Grab the state set of the node, this  will a StateSet if one does not exist
    osg::StateSet *state = srcNode->getOrCreateStateSet();

    //We need to retrieve the Polygon mode of the  state set, and create one if does not have one
    osg::PolygonMode *polyModeObj;

    polyModeObj = dynamic_cast< osg::PolygonMode* >( state->getAttribute( osg::StateAttribute::POLYGONMODE ));

    if ( !polyModeObj )
    {
        polyModeObj = new osg::PolygonMode;
        state->setAttribute( polyModeObj );
    }

    //Now we can set the state to FILL
    polyModeObj->setMode(  osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL );
}

