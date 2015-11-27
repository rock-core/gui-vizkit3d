rock_vizkit_plugin(vizkit3d-viz
    PluginLoader.cpp
    MOC GridVisualization.cpp TextureBoxVisualization.cpp ModelVisualization.cpp
    DEPS vizkit3d
    HEADERS GridVisualization.hpp TextureBoxVisualization.hpp ModelVisualization.hpp
)
