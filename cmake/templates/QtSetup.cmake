### Configure your Qt here
set(QT_SEARCH_PATH /opt)
find_package(Qt4 4.5 REQUIRED QtTest QtThread QtXml QtNetwork QtGui)

# Definitions to use when compiling code that uses Qt
add_definitions(${QT_DEFINITIONS})

# Set your desired Qt-Version here
include_directories(${QT_INCLUDE_DIR})

# Path to a Cmake file that can be included to compile Qt4 applications and libraries
include(${QT_USE_FILE})

set(EXTERNAL_LIBS ${EXTERNAL_LIBS} ${QT_LIBRARIES})


