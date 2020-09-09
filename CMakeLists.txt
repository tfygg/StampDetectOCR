cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_definitions(-std=c++11)

set(CXX_FLAGS "-Wall" "-pedantic")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(OpencvDarknet)
set(OpenCV_DIR "${PROJECT_SOURCE_DIR}/3rdparty/opencv")
find_package(OpenCV 4.1 REQUIRED)
#include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS} /usr/local/opencv410/include)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

# Executables for exercises
add_executable (detect_objects src/demo.cpp src/dataStructures.h src/PutText.cpp src/PutText.h)
target_link_libraries (detect_objects ${OpenCV_LIBRARIES})
