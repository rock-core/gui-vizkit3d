#ifndef __RIGID_BODY_STATE_VISUALIZATION_HPP__
#define __RIGID_BODY_STATE_VISUALIZATION_HPP__

#include <vizkit/VizPlugin.hpp>
#include <Eigen/Geometry>
#include <base/samples/rigid_body_state.h>

namespace vizkit 
{

class RigidBodyStateVisualization : public Vizkit3DPlugin<base::samples::RigidBodyState>
{
        Q_OBJECT
        Q_PROPERTY(bool displayCovariance READ isCovarianceDisplayed WRITE displayCovariance)
        Q_PROPERTY(bool displayCovarianceWithSamples READ isCovarianceDisplayedWithSamples WRITE displayCovarianceWithSamples)
        Q_PROPERTY(bool forcePositionDisplay READ isPositionDisplayForced WRITE setPositionDisplayForceFlag)
        Q_PROPERTY(bool forceOrientationDisplay READ isOrientationDisplayForced WRITE setOrientationDisplayForceFlag)
    public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	RigidBodyStateVisualization();	
	virtual ~RigidBodyStateVisualization();

    protected:
        virtual osg::ref_ptr<osg::Node> createMainNode();
	virtual void updateMainNode(osg::Node* node);
	void updateDataIntern( const base::samples::RigidBodyState& state );
        base::samples::RigidBodyState state;
    
    public: 
        bool isPositionDisplayForced() const;
        void setPositionDisplayForceFlag(bool flag);
        bool isOrientationDisplayForced() const;
        void setOrientationDisplayForceFlag(bool flag);

        void resetModel(double size);
	void resetModelSphere(double size);
	
        void loadModel(std::string const& path);

        /** When using the default body, sets the size of the main sphere,
         * relative to the size of the complete object
         *
         * The default is 0.1
         */
        void setMainSphereSize(double size);

        void displayCovariance(bool enable);
        bool isCovarianceDisplayed() const;
        void displayCovarianceWithSamples(bool enable);
        bool isCovarianceDisplayedWithSamples() const;

        /** Sets the color of the default body model in R, G, B
         *
         * Values must be between 0 and 1
         *
         * If you call it after the plugin got attached, call resetModel to
         * apply the new color
         */
        void setColor(base::Vector3d const& color);
	
	void setColor(const osg::Vec4d& color, osg::Geode* geode);
	
    private:
        bool covariance;
        bool covariance_with_samples;
        base::Vector3d color;
        double main_size;

	osg::Vec3d pos;
	osg::Quat orientation;
	osg::ref_ptr<osg::Node>  body_model;
        osg::ref_ptr<osg::Group> createSimpleBody(double size);
	osg::ref_ptr<osg::Group> createSimpleSphere(double size);

        bool forcePositionDisplay;
        bool forceOrientationDisplay;

};

}
#endif // ROBOT_H
