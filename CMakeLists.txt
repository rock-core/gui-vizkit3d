# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 2.6)

project(vizkit)
set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION "Rock Visualization Library")

# Add command line options for gcc
add_definitions(-Wall -DDebug)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include(CommonSetup)

include($ENV{ROCK_CMAKE_MACROS}/Rock.cmake)
include(RockRuby)
rock_standard_layout()
 
