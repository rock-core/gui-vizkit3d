set(LIB_WIDGET_LOADER ${PROJECT_NAME}widgetloader)

set(SOURCES 
    VizPlugin.cpp
    CompositeViewerQOSG.cpp
    CoordinateFrame.cpp
    GridNode.cpp
    PickHandler.cpp
    QOSGWidget.cpp
    QVizkitWidget.cpp
    WaypointVisualization.cpp
    MotionCommandVisualization.cpp
    TrajectoryVisualization.cpp
    )

set(QT_HEADERS 
    VizPlugin.hpp
    QOSGWidget.hpp 
    QVizkitWidget.hpp 
    CompositeViewerQOSG.hpp 
    PickHandler.hpp
    )

set(HEADERS 
    ${QT_HEADERS}
    CoordinateFrame.hpp
    GridNode.hpp
    MotionCommandVisualization.hpp
    TrajectoryVisualization.hpp
    WaypointVisualization.hpp
    QtThreadedWidget.hpp
    )

# Add your .ui files ot the following line (and uncomment it)
#QT4_WRAP_UI(ui_cpp myproject.ui)
# Add the .h files that define QObject subclasses (that have a Q_OBJECT
# statement)
QT4_WRAP_CPP(moc_cpp ${QT_HEADERS})
QT4_WRAP_CPP(moc_cpp_widget_loader QVizkitWidgetLoader.hpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# We are adding a library here, which is required for the test
# setup, so if you change the setup here
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${moc_cpp} ${ui_cpp})
add_library(${LIB_WIDGET_LOADER} SHARED QVizkitWidgetLoader.cpp ${moc_cpp_widget_loader})
target_link_libraries(${PROJECT_NAME} ${EXTERNAL_LIBS})
target_link_libraries(${LIB_WIDGET_LOADER} ${EXTERNAL_LIBS} ${PROJECT_NAME})

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
INSTALL(FILES ${HEADERS} 
    DESTINATION include/${PROJECT_NAME})
