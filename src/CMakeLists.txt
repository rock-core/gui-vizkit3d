#include(QtSetupSources)

# The main executable 
set(MAIN main.cpp)

# Add other sources 
set(SOURCES dummy.cpp)

# We are adding a library here, which is required for the test
# setup, so if you change the setup here
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${moc_cpp} ${ui_cpp})
target_link_libraries(${PROJECT_NAME} ${EXTERNAL_LIBS})
# Link the defined target (either binary or library, see above) to libraries.
# You must use ${QT_LIBRARIES} if you have enabled Qt support in the root
# CMakeLists.txt, and ${Boost_LIBRARIES} for boost libraries
#
# If you want to use a library defined in this package, just use its name
#

###### Define if you need library or binary
# Note: the library (see below) and executable names can't be the same
#add_library(${PROJECT_NAME}_test SHARED ${SOURCES} ${moc_cpp} ${ui_cpp})
add_executable(${PROJECT_NAME}_bin ${MAIN} ${SOURCES} ${moc_cpp} ${ui_cpp})
target_link_libraries(${PROJECT_NAME}_bin ${EXTERNAL_LIBS})

# Installing relative to you installation prefix 
# e.g. test it via: make install DESTDIR=./tmp
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
