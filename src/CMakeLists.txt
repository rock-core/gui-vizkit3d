rock_find_qt4()
include(RockRuby)

QT4_ADD_RESOURCES(PROPERTY_BROWSER_RESOURCES qtpropertybrowser/qtpropertybrowser.qrc)
rock_library(vizkit3d
    SOURCES
        QtThreadedWidget.cpp
        ColorConversionHelper.cpp
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
        QtThreadedWidget.hpp
        Vizkit3DPlugin.hpp
        Vizkit3DWidget.hpp
        QVizkitMainWindow.hpp
        VizPlugin.hpp
        PickHandler.hpp
        ColorConversionHelper.hpp
        ${HEADERS_EXTRA}
        LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    DEPS_PKGCONFIG openscenegraph eigen3 openscenegraph-osgQt ${DEPS_EXTRA})

rock_library(vizkitwidgetloader
    MOC QVizkitWidgetLoader.cpp
    DEPS vizkit3d
    NOINSTALL) # installs in a non-standard location
rock_library(vizkitmainwindowloader
    MOC QVizkitMainWindowLoader.cpp
    DEPS vizkit3d
    NOINSTALL) # installs in a non-standard location
install(TARGETS vizkitwidgetloader vizkitmainwindowloader
    LIBRARY DESTINATION lib/qt/designer
    ARCHIVE DESTINATION lib/qt/designer)
