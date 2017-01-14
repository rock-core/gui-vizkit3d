include(RockRuby)

# find qt5
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGl REQUIRED)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Add resources from the property browser
qt5_add_resources(PROPERTY_BROWSER_RESOURCES qtpropertybrowser/qtpropertybrowser.qrc)

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
	PickHandler.cpp
	Vizkit3DWidget.cpp
	QVizkitMainWindow.cpp
	qtpropertybrowser/qteditorfactory.cpp
	qtpropertybrowser/qtvariantproperty.cpp
	qtpropertybrowser/qtpropertymanager.cpp
	qtpropertybrowser/qtbuttonpropertybrowser.cpp
	qtpropertybrowser/qtpropertybrowser.cpp
	qtpropertybrowser/qtpropertybrowserutils.cpp
	qtpropertybrowser/qttreepropertybrowser.cpp
	qtpropertybrowser/qtgroupboxpropertybrowser.cpp
	QPropertyBrowserWidget.cpp
        ${PROPERTY_BROWSER_RESOURCES} 
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
        PickHandler.hpp
        ColorConversionHelper.hpp
        CoordinateFrame.hpp
        EnvPluginBase.hpp
        DefaultManipulator.hpp
        ${HEADERS_EXTRA}
	LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    DEPS_CMAKE OpenGL
    DEPS_PKGCONFIG openscenegraph-osgQt openscenegraph ${DEPS_EXTRA})

# setup qt
qt5_use_modules(vizkit3d Widgets OpenGL)

#rock_library(vizkitwidgetloader
#    MOC_QT5 QVizkitWidgetLoader.cpp
#    DEPS vizkit3d
#    NOINSTALL) # installs in a non-standard location
#rock_library(vizkitmainwindowloader
#    MOC QVizkitMainWindowLoader.cpp
#    DEPS vizkit3d
#    NOINSTALL) # installs in a non-standard location
#install(TARGETS vizkitwidgetloader #vizkitmainwindowloader
#    LIBRARY DESTINATION lib/qt/designer
#    ARCHIVE DESTINATION lib/qt/designer)
