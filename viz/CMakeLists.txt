rock_vizkit_plugin(vizkit-base-viz 
    PluginLoader.cpp
    MOC LaserScanVisualization.cpp MotionCommandVisualization.cpp RigidBodyStateVisualization.cpp TrajectoryVisualization.cpp WaypointVisualization.cpp SonarGroundDistanceVisualization.cpp
    DEPS vizkit
    HEADERS LaserScanVisualization.hpp MotionCommandVisualization.hpp RigidBodyStateVisualization.hpp TrajectoryVisualization.hpp WaypointVisualization.hpp SonarGroundDistanceVisualization.hpp
)
