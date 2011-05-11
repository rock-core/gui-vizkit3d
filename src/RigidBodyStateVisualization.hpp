#ifndef __RIGID_BODY_STATE_VISUALIZATION_HPP__
#define __RIGID_BODY_STATE_VISUALIZATION_HPP__

#include <vizkit/VizPlugin.hpp>
#include <Eigen/Geometry>
#include <base/samples/rigid_body_state.h>

namespace vizkit 
{

class RigidBodyStateVisualization : public VizPlugin<base::samples::RigidBodyState>
{
    public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	RigidBodyStateVisualization();	
	virtual ~RigidBodyStateVisualization();

    protected:
        virtual osg::ref_ptr<osg::Node> createMainNode();
	virtual void updateMainNode(osg::Node* node);
	void updateDataIntern( const base::samples::RigidBodyState& state );
        base::samples::RigidBodyState state;
        void resetModel(double size);
        void loadModel(std::string const& path);
        void displayCovarianceWithSamples(bool enable);
	
    private:
        bool covariance_with_samples;
	osg::Vec3d pos;
	osg::Quat orientation;

	osg::ref_ptr<osg::Node> body_model;

        osg::ref_ptr<osg::Geode> createSimpleBody(double size);
};

}
#endif // ROBOT_H
