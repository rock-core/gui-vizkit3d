
rock_find_qt5(Core)
rock_vizkit_plugin(vizkit3d-viz
    SOURCES
    	PluginLoader.cpp
    	GridVisualization.cpp
	TextureBoxVisualization.cpp
	ModelVisualization.cpp
    MOC
	GridVisualization.hpp
	TextureBoxVisualization.hpp
	ModelVisualization.hpp
	PluginLoader.hpp
    DEPS 
    	vizkit3d
)
