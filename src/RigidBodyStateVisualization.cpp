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
    VizPluginRubyAdapter(RigidBodyStateVisualization, base::samples::RigidBodyState, RigidBodyState)
    VizPluginRubyConfig(RigidBodyStateVisualization, double, resetModel);
    VizPluginRubyConfig(RigidBodyStateVisualization, bool, displayCovarianceWithSamples);
}

RigidBodyStateVisualization::~RigidBodyStateVisualization()
{
}

osg::ref_ptr<osg::Geode> RigidBodyStateVisualization::createSimpleBody(double size)
{   
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    osg::ref_ptr<osg::Sphere> sp = new osg::Sphere(osg::Vec3f(0,0,0), size / 10);
    osg::ref_ptr<osg::ShapeDrawable> spd = new osg::ShapeDrawable(sp);
    geode->addDrawable(spd);
    
    //up
    osg::ref_ptr<osg::Cylinder> c1 = new osg::Cylinder(osg::Vec3f(0, 0, size / 2), size / 40, size);
    osg::ref_ptr<osg::ShapeDrawable> c1d = new osg::ShapeDrawable(c1);
    geode->addDrawable(c1d);
    
    //north direction
    osg::ref_ptr<osg::Cylinder> c2 = new osg::Cylinder(osg::Vec3f(0, size / 2, 0), size / 40, size);
    c2->setRotation(osg::Quat(M_PI/2.0, osg::Vec3d(1,0,0)));
    osg::ref_ptr<osg::ShapeDrawable> c2d = new osg::ShapeDrawable(c2);
    c2d->setColor(osg::Vec4f(1.0, 0, 0, 1.0));
    geode->addDrawable(c2d);
  
    //east
    osg::ref_ptr<osg::Cylinder> c3 = new osg::Cylinder(osg::Vec3f(size / 2, 0, 0), size / 40, size);
    c3->setRotation(osg::Quat(M_PI/2.0, osg::Vec3d(0,1,0)));
    osg::ref_ptr<osg::ShapeDrawable> c3d = new osg::ShapeDrawable(c3);
    geode->addDrawable(c3d);
    
    return geode;
}

void RigidBodyStateVisualization::resetModel(double size)
{
    body_model = createSimpleBody(size);
}

void RigidBodyStateVisualization::loadModel(std::string const& path)
{
    osg::ref_ptr<osg::Node> model = osgDB::readNodeFile(path);
    body_model = model;
}

void RigidBodyStateVisualization::displayCovarianceWithSamples(bool enable)
{
    covariance_with_samples = enable;
}

osg::ref_ptr<osg::Node> RigidBodyStateVisualization::createMainNode()
{
    osg::Group* group = new osg::Group;
    group->addChild(new Uncertainty);

    osg::PositionAttitudeTransform* body_pose =
        new osg::PositionAttitudeTransform();
    if (!body_model)
        resetModel(1);
    body_pose->addChild(body_model);
    group->addChild(body_pose);
    return group;
}

void RigidBodyStateVisualization::updateMainNode(osg::Node* node)
{
    osg::Group* group = node->asGroup();

    Uncertainty* uncertainty =
        dynamic_cast<Uncertainty*>(group->getChild(0));
    if (covariance_with_samples)
        uncertainty->showSamples();
    else
        uncertainty->hideSamples();
    osg::PositionAttitudeTransform* body_pose =
        dynamic_cast<osg::PositionAttitudeTransform*>(group->getChild(1));

    osg::Node* body_node = body_pose->getChild(0);
    if (body_node != this->body_model)
        body_pose->setChild(0, this->body_model);

    if(state.hasValidPosition()) {
	pos.set(state.position.x(), state.position.y(), state.position.z());
	body_pose->setPosition(pos);
        uncertainty->setMean(static_cast<Eigen::Vector3d>(state.position));
        uncertainty->setCovariance(static_cast<Eigen::Matrix3d>(state.cov_position));
    }
    if(state.hasValidOrientation()) {
        orientation.set(state.orientation.x(),
                state.orientation.y(),
                state.orientation.z(),
                state.orientation.w());
        body_pose->setAttitude(orientation);
    }
}

void RigidBodyStateVisualization::updateDataIntern( const base::samples::RigidBodyState& state )
{
    this->state = state;
}

}
