#ifndef MOTIONCOMMANDVISUALIZATION_H
#define MOTIONCOMMANDVISUALIZATION_H
#include <vizkit/VizPlugin.hpp>
#include <osg/Shape>
#include <Eigen/Geometry>
#include <osg/Drawable>

namespace vizkit 
{

class MotionCommandVisualization : public VizPlugin<std::pair<double, double> >
{
    public:
	MotionCommandVisualization();	

    protected:
        virtual osg::ref_ptr<osg::Node> createMainNode();
        virtual void updateMainNode( osg::Node* node );
	void updateDataIntern ( const std::pair<double, double> & data );

    private:
	double tv;
	double rv;
	osg::Vec3 robotPosition;
	osg::Quat robotOrientation;
	osg::ref_ptr<osg::Cylinder> motionPointer;
	osg::ref_ptr<osg::Cone> motionPointerHead;
	osg::ref_ptr<osg::Vec3Array> pointsOSG;
	osg::ref_ptr<osg::DrawArrays> drawArrays;
	osg::ref_ptr<osg::Geometry> geom;
	osg::ref_ptr<osg::PositionAttitudeTransform> arrowRotation;
	void drawRotation();
};

}
#endif // MOTIONCOMMANDVISUALIZATION_H
