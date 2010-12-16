#ifndef WAYPOINTVISUALIZATION_H
#define WAYPOINTVISUALIZATION_H

#include <vizkit/VizPlugin.hpp>
#include <base/waypoint.h>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>

namespace vizkit 
{
    
class WaypointVisualization: public VizPluginAdapter<base::Waypoint>
{
    public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	WaypointVisualization();

    protected:
	bool updated;
	virtual void operatorIntern ( osg::Node* node, osg::NodeVisitor* nv );
	virtual void updateDataIntern ( const base::Waypoint& data );
	osg::ref_ptr<osg::PositionAttitudeTransform> waypointPosition;
	base::Waypoint waypoint;
	osg::ref_ptr<osg::Vec3Array> pointsOSG;
	osg::ref_ptr<osg::DrawArrays> drawArrays;
	osg::ref_ptr<osg::Geometry> geom;
};

}
#endif // WAYPOINTVISUALIZATION_H
