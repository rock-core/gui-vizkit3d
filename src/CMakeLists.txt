pkg_check_modules(YAML REQUIRED yaml-cpp)
add_definitions(${YAML_CFLAGS})
include_directories(${YAML_INCLUDE_DIRS})
link_directories(${YAML_LIBRARY_DIRS})

set(LIB_WIDGET_LOADER ${PROJECT_NAME}widgetloader)
set(LIB_MAINWINDOW_LOADER ${PROJECT_NAME}mainwindowloader)
set(TYPELIB_TO_QVARIANT ${PROJECT_NAME}typelib)

set(SOURCES 
    qtpropertybrowser/qteditorfactory.cpp
    qtpropertybrowser/qtvariantproperty.cpp
    qtpropertybrowser/qtpropertymanager.cpp
    qtpropertybrowser/qtbuttonpropertybrowser.cpp
    qtpropertybrowser/qtpropertybrowser.cpp
    qtpropertybrowser/qtpropertybrowserutils.cpp
    qtpropertybrowser/qttreepropertybrowser.cpp
    qtpropertybrowser/qtgroupboxpropertybrowser.cpp
    Vizkit3DPlugin.cpp
    Vizkit3DHelper.cpp
    CompositeViewerQOSG.cpp
    CoordinateFrame.cpp
    GridNode.cpp
    PickHandler.cpp
    QOSGWidget.cpp
    Vizkit3DWidget.cpp
    WaypointVisualization.cpp
    MotionCommandVisualization.cpp
    TrajectoryVisualization.cpp
    MouseManipulationManipulator.cpp
    QtThreadedWidget.cpp
    QVizkitMainWindow.cpp
    Uncertainty.cpp
    RigidBodyStateVisualization.cpp
    QPropertyBrowserWidget.cpp
    )

set(QT_HEADERS 
    qtpropertybrowser/qteditorfactory.h
    qtpropertybrowser/qtvariantproperty.h
    qtpropertybrowser/qtpropertymanager.h
    qtpropertybrowser/qtbuttonpropertybrowser.h
    qtpropertybrowser/qtpropertybrowser.h
    qtpropertybrowser/qtpropertybrowserutils_p.h
    qtpropertybrowser/qttreepropertybrowser.h
    qtpropertybrowser/qtgroupboxpropertybrowser.h
    Vizkit3DPlugin.hpp
    Vizkit3DHelper.hpp
    QOSGWidget.hpp 
    Vizkit3DWidget.hpp 
    CompositeViewerQOSG.hpp 
    PickHandler.hpp
    QVizkitMainWindow.hpp
    QPropertyBrowserWidget.hpp
    )

set(HEADERS 
    ${QT_HEADERS}
    VizPlugin.hpp
    CoordinateFrame.hpp
    GridNode.hpp
    MotionCommandVisualization.hpp
    TrajectoryVisualization.hpp
    WaypointVisualization.hpp
    QtThreadedWidget.hpp
    Uncertainty.hpp
    RigidBodyStateVisualization.hpp
    QVisualizationTestWidget.hpp
    )

# Add your .ui files ot the following line (and uncomment it)
#QT4_WRAP_UI(ui_cpp myproject.ui)
# Add the .h files that define QObject subclasses (that have a Q_OBJECT
# statement)
QT4_WRAP_CPP(moc_cpp ${QT_HEADERS})
QT4_WRAP_CPP(moc_cpp_widget_loader QVizkitWidgetLoader.hpp)
QT4_WRAP_CPP(moc_cpp_mainwindow_loader QVizkitMainWindowLoader.hpp)
QT4_ADD_RESOURCES(PROPERTY_BROWSER_RESOURCES qtpropertybrowser/qtpropertybrowser.qrc)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# We are adding a library here, which is required for the test
# setup, so if you change the setup here
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${moc_cpp} ${ui_cpp} ${PROPERTY_BROWSER_RESOURCES})
add_library(${LIB_WIDGET_LOADER} SHARED QVizkitWidgetLoader.cpp ${moc_cpp_widget_loader})
add_library(${LIB_MAINWINDOW_LOADER} SHARED QVizkitMainWindowLoader.cpp ${moc_cpp_mainwindow_loader})
target_link_libraries(${PROJECT_NAME} ${EXTERNAL_LIBS} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${YAML_LIBRARIES})
target_link_libraries(${LIB_WIDGET_LOADER} ${EXTERNAL_LIBS} ${PROJECT_NAME} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})
target_link_libraries(${LIB_MAINWINDOW_LOADER} ${EXTERNAL_LIBS} ${PROJECT_NAME} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})


# Installing relative to you installation prefix 
# e.g. test it via: make install DESTDIR=./tmp
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(TARGETS ${LIB_WIDGET_LOADER}
    LIBRARY DESTINATION lib/qt/designer
    ARCHIVE DESTINATION lib/qt/designer
)
install(TARGETS ${LIB_MAINWINDOW_LOADER}
    LIBRARY DESTINATION lib/qt/designer
    ARCHIVE DESTINATION lib/qt/designer
)
INSTALL(FILES ${HEADERS} 
    DESTINATION include/${PROJECT_NAME})

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
		  LIBRARY DESTINATION ${RUBY_EXTENSIONS_INSTALL_DIR}
	)
else()
    if (NOT TYPELIB_FOUND)
        message(STATUS "cannot find typelib, the vizkit-ruby bridge is disabled")
    elseif (NOT TYPELIBRUBY_FOUND)
        message(STATUS "cannot find typelib's ruby bindings, the vizkit-ruby bridge is disabled")
    elseif (NOT GEM_FOUND)
        message(STATUS "cannot find the rice gem, the vizkit-ruby bridge is disabled")
    endif()
endif()

