cmake_minimum_required(VERSION 2.6)
project(opencl-train)

#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(OpenCL REQUIRED)

include_directories(${OpenCL_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include/h)
include_directories(${CMAKE_SOURCE_DIR}/include/hpp)


add_definitions(-std=c++11)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SUBDIRS(src)