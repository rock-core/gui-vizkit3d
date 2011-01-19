find_package(Boost COMPONENTS unit_test_framework)
if (Boost_FOUND)
    message(STATUS "boost/unit found, compiling unit test")
    add_executable(testWidget testWidget.cpp)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    target_link_libraries(testWidget vizkit 
        ${Boost_LIBRARIES}
        )   
else(Boost_FOUND)
    message(STATUS "boost/unit not found, disabled unit test")
endif (Boost_FOUND)