cmake_minimum_required(VERSION 3.2)
project (Transformation)

set (CMAKE_CXX_STANDARD 11)
#find_package(Eigen3 REQUIRED)
#include_directories(EIGEN3_INCLUDE_DIR)
include_directories(./../)

add_executable (Transformation main.cpp)
