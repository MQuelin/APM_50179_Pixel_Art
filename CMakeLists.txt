cmake_minimum_required(VERSION 3.18)

PROJECT(
  DePixelLib
  VERSION 0.0.1
  DESCRIPTION "Depixelize pixel art based on a research paper by J. Kopf and D. Lischinski."
  LANGUAGES CXX
)

# For use with IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

add_subdirectory(external/googletest)
add_subdirectory(examples)
add_subdirectory(src)
add_subdirectory(test)
