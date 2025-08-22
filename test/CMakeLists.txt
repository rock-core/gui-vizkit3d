rock_find_qt4(OPTIONAL)
rock_find_qt5(OPTIONAL Core OpenGL Widgets UiPlugin UiTools)

if (ROCK_QT_VERSION_4)
rock_testsuite(testGUI suite.cpp testWidget.cpp
    DEPS vizkit3d
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})

rock_testsuite(testCore suite.cpp testTransformerGraph.cpp
    DEPS vizkit3d
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})
endif()

if (ROCK_QT_VERSION_5)
rock_testsuite(testGUI-qt5 suite.cpp testWidget.cpp
    DEPS vizkit3d-qt5
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})

rock_testsuite(testCore-qt5 suite.cpp testTransformerGraph.cpp
    DEPS vizkit3d-qt5
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})

# build test application
# find_package(Qt5UiTools)
rock_executable(testViewer testViewer.cpp
            DEPS    vizkit3d-qt5
            LIBS    Qt5::Core
                    Qt5::Widgets
                    Qt5::OpenGL
                    Qt5::UiPlugin
                    Qt5::UiTools)
# qt5_use_modules(testViewer Widgets OpenGL UiTools)
endif()
