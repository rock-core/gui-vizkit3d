# Author thomas.roehr@dfki.de
#
# Version 0.2 2010-01-14
#       - add support for searching for multiple gems
# Version 0.1 2010-12-15
# 	- support basic search functionality 
#       - tested to find rice
#
# OUTPUT:
#
# GEM_INCLUDE_DIRS	After successful search contains the include directores
#
# GEM_LIBRARIES		After successful search contains the full path of each found library
# 
#
# Usage: 
# set(Gem_DEBUG TRUE)
# find_package(Gem COMPONENTS rice hoe)
# include_directories(${GEM_INCLUDE_DIRS})
# target_link_libraries(${GEM_LIBRARIES}
#
#

# Check for gem binary first
SET(GEM_EXECUTABLE gem)
EXECUTE_PROCESS(COMMAND which ${GEM_EXECUTABLE} OUTPUT_VARIABLE GEM_EXEC_EXISTS)
if( NOT GEM_EXEC_EXISTS)
	MESSAGE(FATAL_ERROR "Could not find the gem executable - install 'gem' first")
endif()

if(NOT Gem_FIND_COMPONENTS)
	MESSAGE(FATAL_ERROR "If searching for a Gem you have to provide COMPONENTS with the name of the gem") 
endif()

set(GEM_FOUND TRUE)
foreach(Gem_NAME ${Gem_FIND_COMPONENTS})
        # If no gem version has been specified use the highest version found
	if(Gem_FIND_VERSION)
	 # TBD: Perform reg matching on hoe>=1.9 or similar to extract version if available
	endif(Gem_FIND_VERSION)

	set(GEM_HOME ENV{GEM_HOME})

	# Safe output of gem list --local into ${GEM_LOCAL_INFO}
	EXECUTE_PROCESS(COMMAND ${GEM_EXECUTABLE} list --local OUTPUT_VARIABLE GEMS_LOCAL_INFO)

        message(GEM_FOUND)
	if("${GEMS_LOCAL_INFO}" STREQUAL "")
		MESSAGE(FATAL_ERROR "No local gem found. Check your GEM_HOME setting!") 
	else()
		STRING(REGEX MATCH "${Gem_NAME} \\(([0-9.]+)" GEM_INFO "${GEMS_LOCAL_INFO}")
		STRING(REGEX REPLACE ".* \\(([0-9.]+).*" "\\1" GEM_VERSION "${GEM_INFO}")
		MESSAGE("Found gem: ${Gem_NAME} version: ${GEM_VERSION}")
	endif()

	if("${GEM_VERSION}" STREQUAL "")
		MESSAGE(FATAL_ERROR "No local gem found. Check your GEM_HOME setting!") 
	else()
		if("$ENV{GEM_HOME}" STREQUAL "")
			MESSAGE(FATAL_ERROR "GEM_HOME. Check your GEM_HOME setting!") 
		endif()

		list(APPEND GEM_INCLUDE_DIRS "$ENV{GEM_HOME}/gems/${Gem_NAME}-${GEM_VERSION}")

		# Our heuristic to library names available for linking
		# since there is no real standard for where to put the 
		# library
		set(_library_NAMES lib${Gem_NAME}.a
				   lib${Gem_NAME}.so
				   ${Gem_NAME}.a
				   ${Gem_NAME}.so
		)

		set(_library_SEARCH_DIRS
				${GEM_INCLUDE_DIRS}
				${GEM_INCLUDE_DIRS}/lib
				${GEM_INCLUDE_DIRS}/${Gem_NAME}
		)
		
		# Search for an existing library, but only within the gems folder
		foreach(_library_NAME ${_library_NAMES})
			foreach(_library_SEARCH_DIR ${_library_SEARCH_DIRS})
			
				
				find_file(GEM_LIBRARY ${_library_NAME}
						PATHS ${_library_SEARCH_DIR}
						NO_DEFAULT_PATH
				)

				if(Gem_DEBUG)
					message(STATUS "Searching ${_library_NAME} in ${_library_SEARCH_DIR}")	
					message(STATUS " >> ${GEM_LIBRARY}")
				endif()	
				
				if(NOT ("${GEM_LIBRARY}" STREQUAL "GEM_LIBRARY-NOTFOUND"))
					# Use the first library found
					if("${GEM_LIBRARIES}" STREQUAL "")
						list(APPEND GEM_LIBRARIES ${GEM_LIBRARIES} ${GEM_LIBRARY})
					endif()
				endif()
			endforeach()
		endforeach()
                if("${GEM_LIBRARIES}" STREQUAL "")
                    message(FATAL_ERROR 'NOT FOUND')
                    set(GEM_FOUND FALSE)
                endif()
	endif()

	if(Gem_DEBUG)
		message(STATUS "${Gem_NAME} library dir: ${GEM_LIBRARIES}")
		message(STATUS "${Gem_NAME} include dir: ${GEM_INCLUDE_DIRS}")
	endif()
		
	if(Gem_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Gem: ${Gem_NAME} could not be found")
	endif()
endforeach()
