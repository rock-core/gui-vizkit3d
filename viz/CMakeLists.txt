rock_vizkit_plugin(vizkit3d-viz
    PluginLoader.cpp
    MOC GridVisualization.cpp TextureBoxVisualization.cpp RobotVisualization.cpp
    DEPS vizkit3d
    HEADERS GridVisualization.hpp TextureBoxVisualization.hpp RobotVisualization.hpp
)
