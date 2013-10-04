rock_find_qt4()

rock_testsuite(testWidget testWidget.cpp
    DEPS vizkit3d
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})
