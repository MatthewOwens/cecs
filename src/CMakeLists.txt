# Enabling debug symbols
if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Debug)
endif()

list (APPEND CMAKE_CXX_FLAGS "-fPIC -Wall -DGL_GLEXT_PROTOTYPES -Werror=return-type -g -pedantic-errors -std=c++11${CMAKE_CXX_FLAGS}")

message("Current source dir: " ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB SRCS *.cpp *.hpp)
set(GAME_SOURCES ${SRCS})
list(REMOVE_ITEM GAME_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
add_library(GAME_LIB OBJECT ${GAME_SOURCES})

#add_executable(SRPG ${SRCS})
add_executable(cecs main.cpp $<TARGET_OBJECTS:GAME_LIB>)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
message(CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH})
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
find_package(Jsoncpp REQUIRED)

if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  #target_link_libraries(SRPG ${SFML_LIBRARIES})
  target_link_libraries(cecs PRIVATE sfml-graphics sfml-network sfml-audio)
endif()

if(Jsoncpp_FOUND)
	message("Found jsoncpp in " ${Jsoncpp_INCLUDE_DIR})
	include_directories(${Jsoncpp_INCLUDE_DIR})
	target_link_libraries(cecs PRIVATE ${Jsoncpp_LIBRARY})
else()
	message("-- Jsoncpp not found")
	message("  -- jsoncpp_INCLUDE_DIR: " ${Jsoncpp_INCLUDE_DIR})
	message("  -- jsoncpp_LIBRARY: " ${Jsoncpp_LIBRARIES})
endif()
