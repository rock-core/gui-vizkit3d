
rock_find_qt4(OPTIONAL QtOpenGL)
rock_find_qt5(OPTIONAL Core OpenGL Widgets UiPlugin)

set(VIZKIT3D_SOURCES
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
)

set(VIZKIT3D_MOC
        Vizkit3DPlugin.hpp
        Vizkit3DWidget.hpp
        QVizkitMainWindow.hpp
        QPropertyBrowserWidget.hpp
)

set(VIZKIT3D_HEADERS
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
        QPropertyBrowserWidget.hpp
)

if (ROCK_QT_VERSION_4)
include(RockRuby)
    find_package(QtPropertyBrowser-qt4 QUIET)

    if(QtPropertyBrowser-qt4_FOUND)
        message(STATUS "Using external QtPropertyBrowser-qt4")
        set(PROPERTY_BROWSER_TARGET QtPropertyBrowser-qt4)
    else()
        message(STATUS "Using internal qtpropertybrowser")
        QT4_ADD_RESOURCES(PROPERTY_BROWSER_RESOURCES qtpropertybrowser/qtpropertybrowser.qrc)

        set(PROPERTY_BROWSER_SOURCES
                qtpropertybrowser/qteditorfactory.cpp
                qtpropertybrowser/qtvariantproperty.cpp
                qtpropertybrowser/qtpropertymanager.cpp
                qtpropertybrowser/qtbuttonpropertybrowser.cpp
                qtpropertybrowser/qtpropertybrowser.cpp
                qtpropertybrowser/qtpropertybrowserutils.cpp
                qtpropertybrowser/qttreepropertybrowser.cpp
                qtpropertybrowser/qtgroupboxpropertybrowser.cpp
        )

        set(PROPERTY_BROWSER_MOC
                qtpropertybrowser/qteditorfactory.h
                qtpropertybrowser/qtvariantproperty.h
                qtpropertybrowser/qtpropertymanager.h
                qtpropertybrowser/qtbuttonpropertybrowser.h
                qtpropertybrowser/qtpropertybrowser.h
                qtpropertybrowser/qtpropertybrowserutils.h
                qtpropertybrowser/qttreepropertybrowser.h
                qtpropertybrowser/qtgroupboxpropertybrowser.h
        )

        set(PROPERTY_BROWSER_HEADERS
                qtpropertybrowser/qttreepropertybrowser.h
                qtpropertybrowser/qtpropertybrowser.h
                qtpropertybrowser/qtvariantproperty.h
                qtpropertybrowser/qteditorfactory.h
                qtpropertybrowser/qtpropertymanager.h
                qtpropertybrowser/qtpropertybrowserutils.h
        )
    endif()

rock_library(vizkit3d
    SOURCES
        ${VIZKIT3D_SOURCES}
        ${PROPERTY_BROWSER_SOURCES}
        ${PROPERTY_BROWSER_RESOURCES}
    MOC
        ${VIZKIT3D_MOC}
        ${PROPERTY_BROWSER_MOC}
        ${MOC_EXTRA}
    HEADERS
        ${VIZKIT3D_HEADERS}
        ${PROPERTY_BROWSER_HEADERS}
        ${HEADERS_EXTRA}
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} Qt4::QtOpenGL
    DEPS_CMAKE OpenGL
    DEPS_TARGET ${PROPERTY_BROWSER_TARGET}
    DEPS_PKGCONFIG openscenegraph openscenegraph-osgQt osgViz
                   PrimitivesFactory ManipulationClickHandler ${DEPS_EXTRA})

    if(NOT QtPropertyBrowser-qt4_FOUND)
        target_include_directories(vizkit3d PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/qtpropertybrowser)
    endif()

rock_library(vizkitwidgetloader
        QVizkitWidgetLoader.cpp
    MOC QVizkitWidgetLoader.hpp
    DEPS vizkit3d
    NOINSTALL) # installs in a non-standard location
#rock_library(vizkitmainwindowloader
#    MOC QVizkitMainWindowLoader.cpp
#    DEPS vizkit3d
#    NOINSTALL) # installs in a non-standard location

install(TARGETS vizkitwidgetloader #vizkitmainwindowloader
    LIBRARY DESTINATION lib/qt/designer
    ARCHIVE DESTINATION lib/qt/designer)

endif()

if (ROCK_QT_VERSION_5)
    find_package(QtPropertyBrowser QUIET)
    find_package(QtPropertyBrowser-qt5 QUIET)
    if(QtPropertyBrowser_FOUND)
        message(STATUS "Using abhijitkundu QtPropertyBrowser")
        set(QTPROPERTYBROWSER_NAME QtPropertyBrowser)
    elseif(QtPropertyBrowser-qt5_FOUND)
        message(STATUS "Using rock QtPropertyBrowser-qt5")
        set(QTPROPERTYBROWSER_NAME QtPropertyBrowser-qt5)
    else()
        message(SEND_ERROR "Could not find QtPropertyBrowser or QtPropertyBrowser-qt5")
    endif()

pkg_check_modules(OSGQT QUIET osgQt)
pkg_check_modules(OPENSCENEGRAPH_OSGQT5 QUIET openscenegraph-osgQt5)

if(${OPENSCENEGRAPH_OSGQT5_FOUND})
  set(OSGQT_PKGCONFIG_NAME "openscenegraph-osgQt5")
elseif(${OSGQT_FOUND})
  set(OSGQT_PKGCONFIG_NAME "osgQt")
else()
  message(SEND_ERROR "Could not find osgQt or openscenegraph-osgQt5 with pkg-config")
endif()

rock_library(vizkit3d-qt5
    SOURCES
        ${VIZKIT3D_SOURCES}
    MOC5
        ${VIZKIT3D_MOC}
    HEADERS
        QPropertyBrowserWidget.hpp
        ${VIZKIT3D_HEADERS}
    DEPS_TARGET
# QtPropertyBrowser properly sets up its target, fails to setup the plain
# cmake variables(thus cannot be used with DEPS_CMAKE) and does not have a
# pkg-config file(cannot be used with DEPS_PKGCONFIG).
        ${QTPROPERTYBROWSER_NAME}
    LIBS
# Qt has proper targets so potentially could be used with DEPS_TARGET, but
# it also provides proper pkg-config files, that are prefered there. Here,
# we have the qt dependencies hardcoded into the .pc.in and use the cmake
# targets for the cmake side.
        Qt5::Core
        Qt5::Widgets
        Qt5::OpenGL
        Qt5::UiPlugin
        ${Boost_THREAD_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
    DEPS_PKGCONFIG
        osgViz
        ${OSGQT_PKGCONFIG_NAME}
        ManipulationClickHandler
)

rock_library(vizkitwidgetloader-qt5
    SOURCES
        QVizkitWidgetLoader.cpp
    MOC5
        QVizkitWidgetLoader.hpp
    DEPS
        vizkit3d-qt5
    NOINSTALL # installs in a non-standard location
)

rock_library(vizkitmainwindowloader-qt5
    SOURCES
        QVizkitMainWindowLoader.cpp
    DEPS
        vizkit3d-qt5
    NOINSTALL # installs in a non-standard location
)

#normally, these would go into lib/qt5/plugins/designer, but
#in rock, we put them in lib/qt/designer as well, since we already
#have that in QT_PLUGIN_PATH
install(TARGETS vizkitwidgetloader-qt5 vizkitmainwindowloader-qt5
    LIBRARY DESTINATION lib/qt/designer
    ARCHIVE DESTINATION lib/qt/designer)

endif()
