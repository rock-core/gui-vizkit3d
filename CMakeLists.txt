# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.

cmake_minimum_required(VERSION 2.6)

project(vizkit)
set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION "Rock Visualization Library")

# Add command line options for gcc
# add_definitions(-Wall -DDebug)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include(CommonSetup)

# Include headers within source
# If you create subdirectories within source include headers with subdirectory, i.e. 
# #include "subdirectory/myheader.h"
include_directories(${PROJECT_SOURCE_DIR}/external/include)
include_directories(${PROJECT_SOURCE_DIR}/src)

# command line output on, you can also use make VERBOSE=1
# set(CMAKE_VERBOSE_MAKEFILE on)

# check cmake/PkgConfigPackages if you want to add a certain library using 
# pkg-config. It provides examples and you can also add pkg packages there
include(PkgConfigPackages)

# edit cmake/templates/QtSetup if you want to embedd Qt into your project, and uncomment the
# include. Don't forget to include and edit cmake/QtSetupSources in src/CMakeLists.txt
#include(QtSetup)

### Uncomment if you want to use Boost. Use the first find_package stanza to
### search for headers only, and the second one if you need binary libraries
#set(Boost_ADDITIONAL_VERSION 1.40 1.41 1.42 1.43 1.44)
# In case you require a local installation of boost, active the following comment
#set(Boost_LOCALFIRST True)
#find_package(Boost REQUIRED)
#find_package(Boost REQUIRED COMPONENTS graph filesystem)
#include_directories(${Boost_INCLUDE_DIRS})

### Update external libraries that are available
set(EXTERNAL_LIBS ${EXTERNAL_LIBS} ${Boost_LIBRARIES})

# Process CMakeLists.txt in the following subdirectory
add_subdirectory(src)

# Add testing facility based on the boost unittest framework
# (requires boost header files) 
#add_subdirectory(test/unittests)

# Performing some setup step after all variables are know, e.g. such as needed for pkg-config
include(PostSetup)
