# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 2.8.11)
find_package(Rock)
rock_init(vizkit3d 2.0)
rock_standard_layout()
