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

# command line output on, you can also use make VERBOSE=1
# set(CMAKE_VERBOSE_MAKEFILE on)

# Find and setup the projects dependencies
include(Dependencies)

# Process CMakeLists.txt in the following subdirectory
include_directories(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(src)

# Add testing facility based on the boost unittest framework
# (requires boost header files) 
#add_subdirectory(test/unittests)

# Performing some setup step after all variables are know, e.g. such as needed for pkg-config
include(PostSetup)
