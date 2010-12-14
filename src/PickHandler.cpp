#include <osg/Timer>
#include <osg/io_utils>
#include <osg/observer_ptr>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>
#include <osgUtil/LineSegmentIntersector>

#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>

#include "PickHandler.hpp"

PickHandler::PickHandler():
    _mx(0.0),_my(0.0),
    _usePolytopeIntersector(false),
    _useWindowCoordinates(false) {}

PickHandler::~PickHandler() {}

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (!viewer) return false;

    switch(ea.getEventType())
    {
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
    osg::Group* parent = 0;

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
		parent = (nodePath.size()>=2)?dynamic_cast<osg::Group*>(nodePath[nodePath.size()-2]):0;

		//osg::Matrixd l2w = osg::computeLocalToWorld( nodePath );
		//osg::Vec3 global = *intersection.matrix.get() * intersection.localIntersectionPoint;
		osg::Vec3 global = intersection.localIntersectionPoint * *intersection.matrix.get();

		Eigen::Vector3d globalPoint( 
			    global.x(),
			    global.y(),
			    global.z() );
		
		std::cout << globalPoint.transpose() << std::endl;
		emit picked( globalPoint );
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
	    osg::notify(osg::NOTICE)<<"Picked "<<intersection.localIntersectionPoint<<std::endl;

	    osg::NodePath& nodePath = intersection.nodePath;
	    node = (nodePath.size()>=1)?nodePath[nodePath.size()-1]:0;
	    parent = (nodePath.size()>=2)?dynamic_cast<osg::Group*>(nodePath[nodePath.size()-2]):0;

	    // see if the object has a user object which is derived from pickcallback
	    PickedCallback *pc = dynamic_cast<PickedCallback*>(node->getUserData());
	    if( pc )
		pc->picked();

	    if (node) std::cout<<"  Hits "<<node->className()<<" nodePath size"<<nodePath.size()<<std::endl;
	}
    }        
}
