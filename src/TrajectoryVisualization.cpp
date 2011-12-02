#include "TrajectoryVisualization.hpp"
#include <osg/Geometry>
#include <osg/Geode>

namespace vizkit 
{

TrajectoryVisualization::TrajectoryVisualization()
{
    //VizPluginRubyAdapter(TrajectoryVisualization, base::geometry::Spline<3>, Trajectory);
    VizPluginRubyAdapter(TrajectoryVisualization, base::Vector3d, Trajectory);
    VizPluginRubyMethod(TrajectoryVisualization, base::Vector3d, setColor);

    // initialize here so that setColor can be called event
    doClear = false;
    color2 = new osg::Vec4Array;
    geom = new osg::Geometry;
    pointsOSG = new osg::Vec3Array;
}

osg::ref_ptr<osg::Node> TrajectoryVisualization::createMainNode()
{
    geom->setVertexArray(pointsOSG);
    drawArrays = new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, pointsOSG->size() );
    geom->addPrimitiveSet(drawArrays.get());

    // Add the Geometry (Drawable) to a Geode and
    //   return the Geode.
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( geom.get() );

    osg::StateSet* stategeode = geode->getOrCreateStateSet();
    stategeode->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    
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


void TrajectoryVisualization::setColor(const base::Vector3d& color)
{
    setColor(color.x(), color.y(), color.z(), 1.0);
}


void TrajectoryVisualization::clear()
{
    doClear = true;
}

void TrajectoryVisualization::updateMainNode( osg::Node* node )
{   
    std::vector<Eigen::Vector3d>::const_iterator it = points.begin();
    
    pointsOSG->clear();
    for(; it != points.end(); it++) {
	pointsOSG->push_back(osg::Vec3(it->x(), it->y(), it->z()));
    }
    geom->setVertexArray(pointsOSG);
    drawArrays->setCount(pointsOSG->size());
}

void TrajectoryVisualization::updateDataIntern(const base::geometry::Spline3& data)
{
    //needs a copy as getCurveLength is not const
    base::geometry::Spline3 spline = data; 
    
    //delete old trajectory
    points.clear();
    
    if(!data.getSISLCurve())
	return;
    
    //a point every 5 cm
    double stepSize = (spline.getEndParam() - spline.getStartParam()) / (spline.getCurveLength() / 0.05);
    for(double p = spline.getStartParam(); p <= spline.getEndParam(); p += stepSize )
    {
	points.push_back(spline.getPoint(p));
    }
}

void TrajectoryVisualization::updateDataIntern( const base::Vector3d& data )
{
    
    if(doClear)
    {
	points.clear();
	doClear = false;
    }
    Eigen::Vector3d d = data;
    points.push_back(d);
}
}
