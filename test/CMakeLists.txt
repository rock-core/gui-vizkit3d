
rock_testsuite(testGUI suite.cpp testWidget.cpp
    DEPS vizkit3d
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})

rock_testsuite(testCore suite.cpp testTransformerGraph.cpp
    DEPS vizkit3d
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})

# build test application
find_package(QtUiTools)
rock_executable(testViewer testViewer.cpp DEPS vizkit3d)
qt5_use_modules(testViewer Widgets OpenGL UiTools)
