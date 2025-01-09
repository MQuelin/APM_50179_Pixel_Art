cmake_minimum_required(VERSION 3.18)

PROJECT(
  DePixelLib
  VERSION 0.0.1
  DESCRIPTION "Depixelize pixel art based on a research paper by J. Kopf and D. Lischinski."
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)

# For use with IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
add_subdirectory(external/googletest)

find_package(Boost 1.74.0 REQUIRED)

add_subdirectory(examples)
add_subdirectory(src)
add_subdirectory(test)
