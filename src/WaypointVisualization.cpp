#include "WaypointVisualization.hpp"
#include <osg/Geometry>
#include <osg/Geode>

namespace vizkit 
{

WaypointVisualization::WaypointVisualization()
{    
    VizPluginRubyAdapter(WaypointVisualizationWaypoint, base::Waypoint, base::Waypoint)
}

osg::ref_ptr< osg::Node > WaypointVisualization::createMainNode()
{
    //draw a cycle
    geom = new osg::Geometry;
    pointsOSG = new osg::Vec3Array;
   
    drawArrays = new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, pointsOSG->size() );
    // Draw a four-vertex quad from the stored data.
    geom->addPrimitiveSet(drawArrays.get());

    osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array();
    color->push_back( osg::Vec4( 0, 1, 1, 1 ) );
    geom->setColorArray( color.release() );
    geom->setColorBinding( osg::Geometry::BIND_OVERALL );
    
    // Add the Geometry (Drawable) to a Geode and
    //   return the Geode.
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( geom.get() );

    osg::StateSet* stategeode = geode->getOrCreateStateSet();
    stategeode->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    waypointPosition = new osg::PositionAttitudeTransform();
    
    waypointPosition->addChild(geode.release());
    
    return waypointPosition;
}

void WaypointVisualization::updateDataIntern ( const base::Waypoint& data )
{
    waypoint = data;
}

void WaypointVisualization::updateMainNode( osg::Node* node )
{
    double radiusX = waypoint.tol_position;
    double radiusY = waypoint.tol_position;
    int stepsize = 100;
    
    double step = 2*M_PI / stepsize;
    osg::Vec3 startPoint = osg::Vec3(0, radiusY, 0);
    for(int i = 0; i < stepsize; i++) {
        osg::Vec3 endPoint;
        endPoint.x() = -sin(step * i) * radiusX;
        endPoint.y() = cos(step * i) * radiusY;
        pointsOSG->push_back(startPoint);
        pointsOSG->push_back(endPoint);
        startPoint = endPoint;
    }
    drawArrays->setCount(pointsOSG->size());
    geom->setVertexArray(pointsOSG.get());

    waypointPosition->setPosition(osg::Vec3(waypoint.position.x(), waypoint.position.y(), waypoint.position.z()));
}

}
