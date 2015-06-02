rock_find_qt4(QtOpenGL)
include(RockRuby)

QT4_ADD_RESOURCES(PROPERTY_BROWSER_RESOURCES qtpropertybrowser/qtpropertybrowser.qrc)

find_package(OpenSceneGraph REQUIRED osgManipulator osgViewer osgFX osgShadow osgParticle osgTerrain osgDB osgGA osgWidget osgText osgUtil osgQt)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})
link_directories(${OPENSCENEGRAPH_LIBRARY_DIRS})

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
        EnableGLDebugOperation.cpp
        ${PROPERTY_BROWSER_RESOURCES} 
    MOC
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
        ${MOC_EXTRA}
    HEADERS
        TransformerGraph.hpp
        GridNode.hpp
        TextureBox.hpp
        NodeLink.hpp
        AxesNode.hpp
        OsgVisitors.hpp
        QtThreadedWidget.hpp
        Vizkit3DPlugin.hpp
        Vizkit3DWidget.hpp
        QVizkitMainWindow.hpp
        PickHandler.hpp
        ColorConversionHelper.hpp
        CoordinateFrame.hpp
        EnvPluginBase.hpp
        ${HEADERS_EXTRA}
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${OPENSCENEGRAPH_LIBRARIES}
    DEPS_CMAKE OpenGL
    DEPS_PKGCONFIG ${DEPS_EXTRA})

rock_library(vizkitwidgetloader
    MOC QVizkitWidgetLoader.cpp
    DEPS vizkit3d
    NOINSTALL) # installs in a non-standard location
#rock_library(vizkitmainwindowloader
#    MOC QVizkitMainWindowLoader.cpp
#    DEPS vizkit3d
#    NOINSTALL) # installs in a non-standard location
install(TARGETS vizkitwidgetloader #vizkitmainwindowloader
    LIBRARY DESTINATION lib/qt/designer
    ARCHIVE DESTINATION lib/qt/designer)
