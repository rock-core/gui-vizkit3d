find_package(Boost COMPONENTS thread)
rock_find_qt4()
include(RockRuby)


pkg_check_modules(CONTROLDEV "controldev")
if(CONTROLDEV_FOUND)
    SET(DEPS_EXTRA "controldev")
    ADD_DEFINITIONS(-DUSE_3DMOUSE)
    SET(MOC_EXTRA "ConnexionPlugin.cpp")
    SET(HEADERS_EXTRA "ConnexionPlugin.h")
endif()

QT4_ADD_RESOURCES(PROPERTY_BROWSER_RESOURCES qtpropertybrowser/qtpropertybrowser.qrc)
rock_library(vizkit
    SOURCES
        Uncertainty.cpp Vizkit3DHelper.cpp CoordinateFrame.cpp
        QtThreadedWidget.cpp GridNode.cpp MouseManipulationManipulator.cpp
        ${PROPERTY_BROWSER_RESOURCES} 
    MOC
        Vizkit3DPlugin.cpp CompositeViewerQOSG.cpp
        PickHandler.cpp QOSGWidget.cpp
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
        Uncertainty.hpp Vizkit3DHelper.hpp
        QtThreadedWidget.hpp
	QVisualizationTestWidget.hpp
        Vizkit3DPlugin.hpp
        QOSGWidget.hpp
        Vizkit3DWidget.hpp
        QVizkitMainWindow.hpp
        VizPlugin.hpp
        CompositeViewerQOSG.hpp
        PickHandler.hpp
        ${HEADERS_EXTRA}

    DEPS_PLAIN Boost
    DEPS_PKGCONFIG yaml-cpp transformer openscenegraph base-types ${DEPS_EXTRA}
)


rock_library(vizkitwidgetloader
    MOC QVizkitWidgetLoader.cpp
    DEPS vizkit
    NOINSTALL) # installs in a non-standard location
rock_library(vizkitmainwindowloader
    MOC QVizkitMainWindowLoader.cpp
    DEPS vizkit
    NOINSTALL) # installs in a non-standard location
install(TARGETS vizkitwidgetloader vizkitmainwindowloader
    LIBRARY DESTINATION lib/qt/designer
    ARCHIVE DESTINATION lib/qt/designer)

find_package(Gem COMPONENTS rice)
pkg_check_modules(TYPELIB "rtt_typelib-${OROCOS_TARGET}")
pkg_check_modules(TYPELIBRUBY "typelib_ruby")



if (TYPELIB_FOUND AND TYPELIBRUBY_FOUND AND GEM_FOUND)
    rock_ruby_rice_extension(vizkittypelib
	MOC TypelibToQVariant.cpp
        DEPS_PKGCONFIG rtt_typelib-${OROCOS_TARGET} typelib_ruby)
    target_link_libraries(vizkittypelib
        ${QT_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} 
        ${GEM_LIBRARIES}) 

    rock_ruby_rice_extension(TypelibQtAdapter SHARED
	MOC TypelibQtAdapter.cpp
        DEPS_PKGCONFIG rtt_typelib-${OROCOS_TARGET} typelib_ruby)
    target_link_libraries(TypelibQtAdapter
        ${QT_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} 
        ${GEM_LIBRARIES}) 
else()
    if (NOT TYPELIB_FOUND)
        message(STATUS "cannot find typelib, the vizkit-ruby bridge is disabled")
    elseif (NOT TYPELIBRUBY_FOUND)
        message(STATUS "cannot find typelib's ruby bindings, the vizkit-ruby bridge is disabled")
    elseif (NOT GEM_FOUND)
        message(STATUS "cannot find the rice gem, the vizkit-ruby bridge is disabled")
    endif()
endif()

