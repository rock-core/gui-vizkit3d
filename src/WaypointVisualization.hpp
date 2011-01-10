#ifndef WAYPOINTVISUALIZATION_H
#define WAYPOINTVISUALIZATION_H

#include <vizkit/VizPlugin.hpp>
#include <base/waypoint.h>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>

namespace vizkit 
{
    
class WaypointVisualization: public VizPlugin<base::Waypoint>
{
    public:
	WaypointVisualization();

    protected:
        virtual osg::ref_ptr<osg::Node> createMainNode();
	virtual void updateMainNode( osg::Node* node );
	void updateDataIntern ( const base::Waypoint& data );
	osg::ref_ptr<osg::PositionAttitudeTransform> waypointPosition;
	base::Waypoint waypoint;
	osg::ref_ptr<osg::Vec3Array> pointsOSG;
	osg::ref_ptr<osg::DrawArrays> drawArrays;
	osg::ref_ptr<osg::Geometry> geom;
};

}
#endif // WAYPOINTVISUALIZATION_H
