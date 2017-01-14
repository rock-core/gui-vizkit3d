# Instruct CMake to run moc automatically when needed.
find_package(Qt5Core REQUIRED)
set(CMAKE_AUTOMOC ON)

rock_vizkit_plugin(vizkit3d-viz
    PluginLoader.cpp GridVisualization.cpp TextureBoxVisualization.cpp ModelVisualization.cpp
    DEPS vizkit3d
    HEADERS GridVisualization.hpp TextureBoxVisualization.hpp ModelVisualization.hpp PluginLoader.hpp
)
