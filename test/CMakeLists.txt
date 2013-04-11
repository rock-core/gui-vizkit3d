rock_find_qt4()

rock_testsuite(testWidget testWidget.cpp
    DEPS vizkit vizkit-base-viz
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})

rock_init(test_vizkit_widget 0.1)
add_subdirectory(test_vizkit_widget)
