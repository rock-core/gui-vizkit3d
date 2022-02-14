rock_find_qt4()

rock_testsuite(testGUI suite.cpp testWidget.cpp
    DEPS vizkit3d
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})

rock_testsuite(testCore suite.cpp testTransformerGraph.cpp
    DEPS vizkit3d
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})

rock_executable(vizkit3d-bin NOINSTALL
    SOURCES vizkit3d.cpp
    DEPS vizkit3d
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    )
