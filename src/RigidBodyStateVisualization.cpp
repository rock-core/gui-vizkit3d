#include "RigidBodyStateVisualization.hpp"
#include "Uncertainty.hpp"
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>

namespace vizkit 
{

RigidBodyStateVisualization::RigidBodyStateVisualization()
{
}

RigidBodyStateVisualization::~RigidBodyStateVisualization()
{
}

osg::ref_ptr<osg::Geode> RigidBodyStateVisualization::createSimpleBody()
{   
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    osg::ref_ptr<osg::Sphere> sp = new osg::Sphere(osg::Vec3f(0,0,0), 0.3);
    osg::ref_ptr<osg::ShapeDrawable> spd = new osg::ShapeDrawable(sp);
    geode->addDrawable(spd);
    
    //up
    osg::ref_ptr<osg::Cylinder> c1 = new osg::Cylinder(osg::Vec3f(0, 0, 0.5), 0.05, 1.0);
    osg::ref_ptr<osg::ShapeDrawable> c1d = new osg::ShapeDrawable(c1);
    geode->addDrawable(c1d);
    
    //north direction
    osg::ref_ptr<osg::Cylinder> c2 = new osg::Cylinder(osg::Vec3f(0, 0.5, 0), 0.05, 1.0);
    c2->setRotation(osg::Quat(M_PI/2.0, osg::Vec3d(1,0,0)));
    osg::ref_ptr<osg::ShapeDrawable> c2d = new osg::ShapeDrawable(c2);
    c2d->setColor(osg::Vec4f(1.0, 0, 0, 1.0));
    geode->addDrawable(c2d);
  
    //east
    osg::ref_ptr<osg::Cylinder> c3 = new osg::Cylinder(osg::Vec3f(0.5, 0, 0), 0.05, 1.0);
    c3->setRotation(osg::Quat(M_PI/2.0, osg::Vec3d(0,1,0)));
    osg::ref_ptr<osg::ShapeDrawable> c3d = new osg::ShapeDrawable(c3);
    geode->addDrawable(c3d);
    
    return geode;
}

void RigidBodyStateVisualization::resetModel()
{
    body_model = createSimpleBody();
}

void RigidBodyStateVisualization::loadModel(std::string const& path)
{
    osg::ref_ptr<osg::Node> model = osgDB::readNodeFile(path);
    body_model = model;
}

osg::ref_ptr<osg::Node> RigidBodyStateVisualization::createMainNode()
{
    osg::PositionAttitudeTransform* pose = new osg::PositionAttitudeTransform();
    pose->addChild(new Uncertainty);
    body_model = createSimpleBody();
    return pose;
}

void RigidBodyStateVisualization::updateMainNode(osg::Node* node)
{
    osg::PositionAttitudeTransform* pose =
        dynamic_cast<osg::PositionAttitudeTransform*>(node->asTransform());
    Uncertainty* uncertainty =
        dynamic_cast<Uncertainty*>(pose->getChild(0));
    osg::Node* body_node = pose->getChild(1);
    if (body_node != this->body_model)
        pose->setChild(1, this->body_model);

    if(state.hasValidPosition()) {
	pos.set(state.position.x(), state.position.y(), state.position.z());
	pose->setPosition(pos);
        uncertainty->setMean(static_cast<Eigen::Vector3d>(state.position));
        uncertainty->setCovariance(static_cast<Eigen::Matrix3d>(state.cov_position));
    }
    if(state.hasValidOrientation()) {
        orientation.set(state.orientation.x(),
                state.orientation.y(),
                state.orientation.z(),
                state.orientation.w());
        pose->setAttitude(orientation);
    }
}

void RigidBodyStateVisualization::updateDataIntern( const base::samples::RigidBodyState& state )
{
    this->state = state;
}

}
