rock_find_qt5(OpenGL)
rock_vizkit_plugin(vizkit3d-viz
    MOC GridVisualization.cpp TextureBoxVisualization.cpp ModelVisualization.cpp PluginLoader.cpp
    DEPS vizkit3d
    HEADERS GridVisualization.hpp TextureBoxVisualization.hpp ModelVisualization.hpp PluginLoader.hpp
)
