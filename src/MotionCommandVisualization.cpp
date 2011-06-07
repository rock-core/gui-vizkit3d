#include "MotionCommandVisualization.hpp"
#include <osg/ShapeDrawable>
#include <osg/Shape>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>

namespace vizkit 
{

MotionCommandVisualization::MotionCommandVisualization()
{
    tv = 0;
    rv = 0;
    VizPluginRubyAdapter(MotionCommandVisualization, base::MotionCommand2D, MotionCommand)
}

osg::ref_ptr< osg::Node > MotionCommandVisualization::createMainNode()
{
    positionTransformation = new osg::PositionAttitudeTransform();
    osg::StateSet* state = positionTransformation->getOrCreateStateSet();
    state->setMode( GL_LIGHTING, osg::StateAttribute::ON );

    arrowRotation = new osg::PositionAttitudeTransform();
    
    motionPointer = new osg::Cylinder(osg::Vec3f(0, 0.5, 0), 0.05, 1.0);
    motionPointer->setRotation(osg::Quat(M_PI/2.0, osg::Vec3d(1,0,0)));
    osg::ref_ptr<osg::ShapeDrawable> c2d = new osg::ShapeDrawable(motionPointer);
    c2d->setColor(osg::Vec4f(0, 0, 1.0, 1.0));
    osg::ref_ptr<osg::Geode> c2g = new osg::Geode();
    c2g->addDrawable(c2d);
    
    arrowRotation->addChild(c2g.release());
    
    //head of the arrow
    motionPointerHead = new osg::Cone(osg::Vec3f(0, 1.0, 0), 0.15, 0.2);
    motionPointerHead->setRotation(osg::Quat(-M_PI/2.0, osg::Vec3d(1,0,0)));
    osg::ref_ptr<osg::ShapeDrawable> shc = new osg::ShapeDrawable(motionPointerHead);
    shc->setColor(osg::Vec4f(0, 0, 1.0, 1.0));
    osg::ref_ptr<osg::Geode> shg = new osg::Geode();
    shg->addDrawable(shc);
    
    arrowRotation->addChild(shg.release());
    
    positionTransformation->addChild(arrowRotation.get());
    
    //draw a cycle around the robot while turning
    geom = new osg::Geometry;
    pointsOSG = new osg::Vec3Array;

    geom->setVertexArray(pointsOSG.get());
    
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
  
    positionTransformation->addChild(geode.release());    
    return positionTransformation;
}


void MotionCommandVisualization::updateMainNode( osg::Node* node )
{
    //DEBUG REMOVE ME
    if(tv < 0.1)
	tv = 1;
    
    motionPointer->setHeight(tv);
    motionPointer->setCenter(osg::Vec3(0, tv/2.0, 0));
    
    motionPointerHead->setCenter(osg::Vec3(0,tv, 0));
    
    positionTransformation->setPosition(robotPosition);
    positionTransformation->setAttitude(robotOrientation);
    arrowRotation->setAttitude(osg::Quat(rv, osg::Vec3d(0,0,1)));
    
    drawRotation();
}

void MotionCommandVisualization::updateDataIntern ( const std::pair< base::MotionCommand2D, base::Pose >& data )
{
    tv = data.first.translation;
    rv = data.first.rotation;
    
    robotPosition = osg::Vec3(data.second.position.x(), data.second.position.y(), data.second.position.z());
    robotOrientation = osg::Quat(data.second.orientation.x(), data.second.orientation.y(), data.second.orientation.z(), data.second.orientation.w());
}


void MotionCommandVisualization::drawRotation() 
{
    double radius = 1.0;
    int stepsize = 100;
    
    pointsOSG->clear();

    double step = rv / stepsize;
    osg::Vec3 startPoint = osg::Vec3(0, radius, 0);
    for(int i = 0; i < stepsize; i++) {
	osg::Vec3 endPoint;
	endPoint.x() = -sin(step * i) * radius;
	endPoint.y() = cos(step * i) * radius;
	pointsOSG->push_back(startPoint);
	pointsOSG->push_back(endPoint);
	startPoint = endPoint;
    }
    drawArrays->setCount(pointsOSG->size());
    geom->setVertexArray(pointsOSG);
}

}
