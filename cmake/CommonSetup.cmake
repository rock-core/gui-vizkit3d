set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/templates")

# In order to allow all header to use an unified inclusion schema
# #include <project-name/project-headerfile.h>

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/include/)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/include/${PROJECT_NAME})
include_directories(${PROJECT_BINARY_DIR}/include/)

if(DEFINED CMAKE_BUILD_TYPE)
#   set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of
# build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug
# Release RelWithDebInfo MinSizeRel.")
else()
   ##### Build types #################################################
   # single-configuration generator like Makefile generator creates following variables per default
   #
   # None (CMAKE_C_FLAGS or CMAKE_CXX_FLAGS used)
   # Debug (CMAKE_C_FLAGS_DEBUG or CMAKE_CXX_FLAGS_DEBUG)
   # Release (CMAKE_C_FLAGS_RELEASE or CMAKE_CXX_FLAGS_RELEASE)
   # RelWithDebInfo (CMAKE_C_FLAGS_RELWITHDEBINFO or CMAKE_CXX_FLAGS_RELWITHDEBINFO
   # MinSizeRel (CMAKE_C_FLAGS_MINSIZEREL or CMAKE_CXX_FLAGS_MINSIZEREL) 
   ####################################################################
   set(CMAKE_BUILD_TYPE Debug)
# Cache only works for the cmake gui
# CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
endif()

message(STATUS "Build type set to: " ${CMAKE_BUILD_TYPE})

##### Add doxygen support ###################################################
include(FindDoxygen) #sets DOXYGEN_EXECUTABLE
if(DOXYGEN_EXECUTABLE)
    # uses
    # PROJECT_NAME           = @PROJECT_NAME@
    # PROJECT_NUMBER         = @PROJECT_VERSION@
    # OUTPUT_DIRECTORY       = @PROJECT_BINARY_DIR@/doc
    # INPUT                  = @PROJECT_SOURCE_DIR@/src
    # input output @ONLY: replace @VAR@ in the input file with the cmake variables
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/doc/Doxyfile.in ${PROJECT_SOURCE_DIR}/doc/Doxyfile @ONLY)
    # documentation can be generated with 'make doc'
    ADD_CUSTOM_TARGET(doc ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/doc/Doxyfile)
    # generates documentation with cmake
    # QUIET mode is enabled in the configuration file (QUIET = YES)
    # If you want to autogenerate you documentation use set(AUTOGENERATE_DOC True) in the main 
    # CMakeList.txt file
    if(DEFINED AUTOGENERATE_DOC)
	    EXECUTE_PROCESS(COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/doc/Doxyfile )
    endif()
endif(DOXYGEN_EXECUTABLE)
##### End doxygen support ###################################################

#### Common installation #################################################
## bin/
## Specifies a common place where CMake should put all the executables.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

## lib/
# Specifies a common place where CMake should put all the libraries
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

## include/
# does a recursive copy of the src directory for all files matching
# the pattern and it maintains the directory-tree
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/ DESTINATION include/${PROJECT_NAME}
        FILES_MATCHING PATTERN "*.h")

## configuration/
# COPY Configuration files into build directory. Workaround: remove the .pc.in
# file, as we don't want to install it
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/configuration ${PROJECT_BINARY_DIR}/configuration)

# install configuration files
install(DIRECTORY ${PROJECT_BINARY_DIR}/configuration/ DESTINATION configuration/${PROJECT_NAME}
	        FILES_MATCHING PATTERN "*" 
	                       PATTERN "*.pc" EXCLUDE)

# Workaround: Cleanup the in file from build directory
# First making sure we are not in the source directory, otherwise we can delete the in files
string(COMPARE NOTEQUAL "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}" BUILDING_IN_SRC_DIR)
if ( ${BUILDING_IN_SRC_DIR} )
		execute_process(COMMAND ${CMAKE_COMMAND} -E remove -f ${PROJECT_BINARY_DIR}/configuration/${PROJECT_NAME}.pc.in)
else ( ${BUILDING_IN_SRC_DIR} )
    message("WARNING: Your are building in the source directory. Recommending to abort and using a designated build directory.") 
endif( ${BUILDING_IN_SRC_DIR} )

# scripts/
# Install the scripts
install(DIRECTORY ${PROJECT_SOURCE_DIR}/scripts/ DESTINATION scripts/${PROJECT_NAME} )
