rock_find_qt4()

rock_testsuite(testWidget testWidget.cpp
    DEPS vizkit vizkit-base-viz)

rock_init(test_vizkit_widget 0.1)
add_subdirectory(test_vizkit_widget)
