# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 3.10)
project(vizkit3d
	VERSION 0.2
	DESCRIPTION "vizkit3d visualization library")
find_package(Rock)
rock_init()
rock_feature(NOCURDIR)

rock_find_cmake(Boost REQUIRED COMPONENTS thread system)
rock_standard_layout()
