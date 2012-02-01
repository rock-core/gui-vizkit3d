include(Rock) 
QT4_WRAP_CPP(moc_cpp WaypointVisualization.hpp
		    RigidBodyStateVisualization.hpp)

rock_library_common(vizkit-base-viz 
    LaserScanVisualization.cpp 
    MotionCommandVisualization.cpp
    RigidBodyStateVisualization.cpp
    TrajectoryVisualization.cpp
    WaypointVisualization.cpp
    PluginLoader.cpp
    ${moc_cpp}
    DEPS
    vizkit
    )

install(TARGETS vizkit-base-viz
    LIBRARY DESTINATION lib)

install(FILES     
    LaserScanVisualization.hpp
    MotionCommandVisualization.hpp
    RigidBodyStateVisualization.hpp
    TrajectoryVisualization.hpp
    WaypointVisualization.hpp
    DESTINATION include/vizkit)