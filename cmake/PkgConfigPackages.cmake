# This file provides a template for pkg-config packages to be easily embedded into the 
# general project layout
# 
# After adding a project make sure you add the LIBRARIES to the EXTERNAL_LIBS variable
# (or you have to link it explicitly to your desired target in src/CMakeList.txt

# Using PkgConfig
include(FindPkgConfig)

#pkg_check_modules(MyLib "mylib")

# The values _INCLUDE_DIRS _LIBRARY_DIRS and _LIBRARIES are created automatically
include_directories(${MyLib_INCLUDE_DIRS})
link_directories(${MyLib_LIBRARY_DIRS})

# Allowing to use EXTERNAL_LIBS variable to link the libraries to you target, e.g.:
# target_link_libaries(yourtarget ${EXTERNAL_LIBS})
set(EXTERNAL_LIBS ${EXTERNAL_LIBS} ${MyLib_LIBRARIES})


