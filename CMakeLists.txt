# Simple Cmake Setup, utilises a single subdirectory (src) and adds all C++
# source files to the project

# cmake is forwards compatable, but not always backwards compatable. So a
# minimum version must be specified.
cmake_minimum_required(VERSION 3.1)	# for now I'm just using the version I have installed
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

project(cecs) # defining the project's name

add_subdirectory(src)	# adding a subdirectory, cmake will look here for an
						# additional CMakeLists.txt file.
add_subdirectory(tests)
