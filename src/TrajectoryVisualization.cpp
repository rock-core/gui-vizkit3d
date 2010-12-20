#include "TrajectoryVisualization.hpp"
#include <osg/Geometry>
#include <osg/Geode>

namespace vizkit 
{

TrajectoryVisualization::TrajectoryVisualization()
{
}

osg::ref_ptr<osg::Node> TrajectoryVisualization::createMainNode()
{
    doClear = false;
    color2 = new osg::Vec4Array;
    geom = new osg::Geometry;
    pointsOSG = new osg::Vec3Array;

    geom->setVertexArray(pointsOSG);
    drawArrays = new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, pointsOSG->size() );
    // Draw a four-vertex quad from the stored data.
    geom->addPrimitiveSet(drawArrays.get());

    // Add the Geometry (Drawable) to a Geode and
    //   return the Geode.
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( geom.get() );
    
    return geode;
}

void TrajectoryVisualization::setColor(double r, double g, double b, double a)
{
    // set colors
    color2->clear();
    color2->push_back( osg::Vec4( r, g, b, a ) );
    geom->setColorArray( color2 );
    geom->setColorBinding( osg::Geometry::BIND_OVERALL );
}

void TrajectoryVisualization::clear()
{
    doClear = true;
}

void TrajectoryVisualization::updateMainNode( osg::Node* node )
{   
    std::vector<Eigen::Vector3d>::const_iterator it = points.begin();
    
    pointsOSG->clear();
    //push first point as start point
    if(it != points.end()) {
	pointsOSG->push_back(osg::Vec3(it->x(), it->y(), it->z()));
	it++;
    }
    
    for(; it != points.end(); it++) {
	//push tow point, as osg wants an start end endpoint for lines
	//endpoint
	pointsOSG->push_back(osg::Vec3(it->x(), it->y(), it->z()));
	//startpoint
	pointsOSG->push_back(osg::Vec3(it->x(), it->y(), it->z()));
    }
    drawArrays->setCount(pointsOSG->size());
    geom->setVertexArray(pointsOSG);
}


void TrajectoryVisualization::updateDataIntern( const Eigen::Vector3d& data )
{
    if(doClear)
    {
	points.clear();
	doClear = false;
    }
    
    points.push_back(data);
}
}
