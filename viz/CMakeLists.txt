rock_vizkit_plugin(vizkit-base-viz
    PluginLoader.cpp
    MOC GridVisualization.cpp LaserScanVisualization.cpp MotionCommandVisualization.cpp RigidBodyStateVisualization.cpp TrajectoryVisualization.cpp WaypointVisualization.cpp SonarGroundDistanceVisualization.cpp PointcloudVisualization.cpp
    DEPS vizkit
    HEADERS GridVisualization.hpp LaserScanVisualization.hpp MotionCommandVisualization.hpp RigidBodyStateVisualization.hpp TrajectoryVisualization.hpp WaypointVisualization.hpp SonarGroundDistanceVisualization.hpp PointcloudVisualization.hpp
)
