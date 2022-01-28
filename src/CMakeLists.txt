rock_find_qt5(Core OpenGL Widgets UiPlugin REQUIRED)
find_package(QtPropertyBrowser REQUIRED)
find_package(Boost COMPONENTS Thread REQUIRED)

rock_library(vizkit3d
    SOURCES
    	QtThreadedWidget.cpp
	NodeLink.cpp
	ColorConversionHelper.cpp
	GridNode.cpp
	TextureBox.cpp
	AxesNode.cpp
	OsgVisitors.cpp
	TransformerGraph.cpp
	CoordinateFrame.cpp
	DefaultManipulator.cpp
	EnableGLDebugOperation.cpp
	Vizkit3DPlugin.cpp 
	Vizkit3DWidget.cpp
	QVizkitMainWindow.cpp
	QPropertyBrowserWidget.cpp
    HEADERS
    	TransformerGraph.hpp
        GridNode.hpp
        TextureBox.hpp
        NodeLink.hpp
        AxesNode.hpp
        OsgVisitors.hpp
        QtThreadedWidget.hpp
        Vizkit3DBase.hpp
	Vizkit3DPlugin.hpp
	Vizkit3DWidget.hpp
	QVizkitMainWindow.hpp
	ColorConversionHelper.hpp
	CoordinateFrame.hpp
	EnvPluginBase.hpp
	DefaultManipulator.hpp
    DEPS
        QtPropertyBrowser
	Qt5::Core	
    	Qt5::Widgets
	Qt5::OpenGL
	Qt::UiPlugin
	Boost::thread
    DEPS_PKGCONFIG
    	osgViz
	osgQt
	ManipulationClickHandler
)

rock_library(vizkitwidgetloader
    SOURCES
    	QVizkitWidgetLoader.cpp
    DEPS
    	vizkit3d
    NOINSTALL # installs in a non-standard location
)

rock_library(vizkitmainwindowloader
    SOURCES
        QVizkitMainWindowLoader.cpp
    DEPS
        vizkit3d
    NOINSTALL # installs in a non-standard location
)
    
install(TARGETS vizkitwidgetloader #vizkitmainwindowloader
    LIBRARY DESTINATION lib/qt/designer
    ARCHIVE DESTINATION lib/qt/designer)
