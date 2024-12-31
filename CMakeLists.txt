cmake_minimum_required(VERSION 3.18)

PROJECT(DePixelLib CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED)

add_subdirectory(examples)
add_subdirectory(src)
# add_subdirectory(test)
