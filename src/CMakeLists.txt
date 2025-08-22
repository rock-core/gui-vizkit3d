
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
        Vizkit3DPluginDefaultSettings.hpp
        Vizkit3DWidget.hpp
        QVizkitMainWindow.hpp
        ColorConversionHelper.hpp
        CoordinateFrame.hpp
        EnvPluginBase.hpp
        DefaultManipulator.hpp
        QPropertyBrowserWidget.hpp
)

include(RockQt)

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
endif()

if(ROCK_QT_VERSION_5)

pkg_check_modules(OSGQT QUIET osgQt)
pkg_check_modules(OPENSCENEGRAPH_OSGQT5 QUIET openscenegraph-osgQt5)

if(${OPENSCENEGRAPH_OSGQT5_FOUND})
  set(OSGQT_PKGCONFIG_NAME "openscenegraph-osgQt5")
elseif(${OSGQT_FOUND})
  set(OSGQT_PKGCONFIG_NAME "osgQt")
else()
  message(SEND_ERROR "Could not find osgQt or openscenegraph-osgQt5 with pkg-config")
endif()

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

endif()


rock_qt_library(
    TARGETPREFIX vizkit3d
    TARGET_QT4 vizkit3d              # do not suffix
    SOURCES
        ${VIZKIT3D_SOURCES}
    SOURCES_QT4
        ${PROPERTY_BROWSER_SOURCES}
        ${PROPERTY_BROWSER_RESOURCES}
    MOC
        ${VIZKIT3D_MOC}
    MOC_QT4
        ${PROPERTY_BROWSER_MOC}
    HEADERS
        ${VIZKIT3D_HEADERS}
    HEADERS_QT4
        ${PROPERTY_BROWSER_HEADERS}
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY}
# Qt has proper targets so potentially could be used with DEPS_TARGET, but
# it also provides proper pkg-config files, that are prefered there. Here,
# we have the qt dependencies hardcoded into the .pc.in and use the cmake
# targets for the cmake side.
    LIBS_QT4 Qt4::QtOpenGL
    LIBS_QT5 Qt5::Core Qt5::Widgets Qt5::OpenGL Qt5::UiPlugin
    DEPS_CMAKE OpenGL
    DEPS_TARGET_QT4 ${PROPERTY_BROWSER_TARGET}
# QtPropertyBrowser properly sets up its target, fails to setup the plain
# cmake variables(thus cannot be used with DEPS_CMAKE) and does not have a
# pkg-config file(cannot be used with DEPS_PKGCONFIG).
    DEPS_TARGET_QT5 ${QTPROPERTYBROWSER_NAME}
    DEPS_PKGCONFIG osgViz ManipulationClickHandler
    DEPS_PKGCONFIG_QT4 openscenegraph openscenegraph-osgQt
                   PrimitivesFactory
    DEPS_PKGCONFIG_QT5
        ${OSGQT_PKGCONFIG_NAME}
    )

if (ROCK_QT_VERSION_4)
    if(NOT QtPropertyBrowser-qt4_FOUND)
        target_include_directories(vizkit3d PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/qtpropertybrowser)
    endif()
endif()

rock_qt_library(
    TARGETPREFIX vizkitwidgetloader
    QT4_SUFFIX ""
    SOURCES
        QVizkitWidgetLoader.cpp
    MOC
        QVizkitWidgetLoader.hpp
    DEPS_QT
        vizkit3d
    NOINSTALL # installs in a non-standard location
)

rock_qt_library(
    TARGETPREFIX vizkitmainwindowloader
    QT4_SUFFIX ""
    SOURCES
        QVizkitMainWindowLoader.cpp
    DEPS_QT
        vizkit3d
    NOINSTALL # installs in a non-standard location
)

if(TARGET vizkitwidgetloader)
    install(TARGETS vizkitwidgetloader
        LIBRARY DESTINATION lib/qt/designer
        ARCHIVE DESTINATION lib/qt/designer)
endif()
if(TARGET vizkitmainwindowloader)
    install(TARGETS vizkitmainwindowloader
        LIBRARY DESTINATION lib/qt/designer
        ARCHIVE DESTINATION lib/qt/designer)
endif()
#normally, these would go into lib/qt5/plugins/designer, but
#in rock, we put them in lib/qt/designer as well, since we already
#have that in QT_PLUGIN_PATH
if(TARGET vizkitwidgetloader-qt5)
    install(TARGETS vizkitwidgetloader-qt5
        LIBRARY DESTINATION lib/qt/designer
        ARCHIVE DESTINATION lib/qt/designer)
endif()
if(TARGET vizkitmainwindowloader-qt5)
    install(TARGETS vizkitmainwindowloader-qt5
        LIBRARY DESTINATION lib/qt/designer
        ARCHIVE DESTINATION lib/qt/designer)
endif()

