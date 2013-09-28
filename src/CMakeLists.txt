find_package(Boost COMPONENTS thread)
rock_find_qt4()
include(RockRuby)

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

    HEADERS
        Uncertainty.hpp Vizkit3DHelper.hpp
        QtThreadedWidget.hpp
        Vizkit3DPlugin.hpp
        QOSGWidget.hpp
        Vizkit3DWidget.hpp
        QVizkitMainWindow.hpp
        VizPlugin.hpp
        CompositeViewerQOSG.hpp

    DEPS_PLAIN Boost
    DEPS_PKGCONFIG yaml-cpp transformer openscenegraph base-types)

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
pkg_check_modules(TYPELIB "rtt-typelib")
pkg_check_modules(TYPELIBRUBY "typelib_ruby")

if (TYPELIB_FOUND AND TYPELIBRUBY_FOUND AND GEM_FOUND)
	include_directories(${GEM_INCLUDE_DIRS})

	include_directories(${TYPELIB_INCLUDE_DIRS})
	link_directories(${TYPELIB_LIBRARY_DIRS})
	add_definitions(${TYPELIB_CFLAGS_OTHER})

	include_directories(${TYPELIBRUBY_INCLUDE_DIRS})
	link_directories(${TYPELIBRUBY_LIBRARY_DIRS})

	QT4_WRAP_CPP(rqt_MOC_SRCS TypelibToQVariant.hpp)
	ROCK_RUBY_EXTENSION(${TYPELIB_TO_QVARIANT} SHARED
			TypelibToQVariant.cpp ${rqt_MOC_SRCS})
	target_link_libraries(${TYPELIB_TO_QVARIANT} 
		                          ${QT_LIBRARIES}
		                          ${QT_QTCORE_LIBRARY} 
		                          ${QT_QTGUI_LIBRARY} 
		                          ${GEM_LIBRARIES}
		                          ${TYPELIB_LIBRARIES}
		                          ${TYPELIBRUBY_LIBRARIES}) 

	install(TARGETS ${TYPELIB_TO_QVARIANT}
		  LIBRARY DESTINATION ${RUBY_EXTENSIONS_INSTALL_DIR})

	QT4_WRAP_CPP(rqta_MOC_SRCS TypelibQtAdapter.h)
	ROCK_RUBY_EXTENSION(TypelibQtAdapter SHARED
			TypelibQtAdapter.cpp  ${rqta_MOC_SRCS})
	target_link_libraries(TypelibQtAdapter 
				${QT_LIBRARIES}
				${QT_QTCORE_LIBRARY} 
				${QT_QTGUI_LIBRARY} 
				${GEM_LIBRARIES}
				${TYPELIB_LIBRARIES}
				${TYPELIBRUBY_LIBRARIES}) 

	install(TARGETS TypelibQtAdapter
		  LIBRARY DESTINATION ${RUBY_EXTENSIONS_INSTALL_DIR})
else()
    if (NOT TYPELIB_FOUND)
        message(STATUS "cannot find typelib, the vizkit-ruby bridge is disabled")
    elseif (NOT TYPELIBRUBY_FOUND)
        message(STATUS "cannot find typelib's ruby bindings, the vizkit-ruby bridge is disabled")
    elseif (NOT GEM_FOUND)
        message(STATUS "cannot find the rice gem, the vizkit-ruby bridge is disabled")
    endif()
endif()

