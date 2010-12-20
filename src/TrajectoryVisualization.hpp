#ifndef TRAJECTORYVISUALISATION_H
#define TRAJECTORYVISUALISATION_H
#include <Eigen/Geometry>
#include <vector>
#include <vizkit/VizPlugin.hpp>
#include <osg/Geometry>

namespace vizkit 
{

class TrajectoryVisualization: public VizPlugin<Eigen::Vector3d>
{
    public:
	TrajectoryVisualization();    
	void setColor(double r, double g, double b, double a);
	void clear();
	
    protected:
	virtual osg::ref_ptr<osg::Node> createMainNode();
	virtual void updateMainNode( osg::Node* node );
	void updateDataIntern( const Eigen::Vector3d& data );
	
    private:
	osg::Vec4Array *color2; 
	bool doClear;

	std::vector<Eigen::Vector3d> points;
	osg::ref_ptr<osg::Vec3Array> pointsOSG;
	osg::ref_ptr<osg::DrawArrays> drawArrays;
	osg::ref_ptr<osg::Geometry> geom;
};

}
#endif // TRAJECTORYVISUALISATION_H
