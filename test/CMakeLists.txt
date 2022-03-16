rock_find_qt5()

rock_testsuite(testGUI suite.cpp testWidget.cpp
    DEPS vizkit3d
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})

rock_testsuite(testCore suite.cpp testTransformerGraph.cpp
    DEPS vizkit3d
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})
