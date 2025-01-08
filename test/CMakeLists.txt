# Add executables
#add_executable(test test.cpp)
add_executable(test_graph test_graph.cpp)

# Find necessary packages
find_package(OpenCV REQUIRED)
find_package(xtensor REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include/depixel_lib ${OpenCV_INCLUDE_DIRS} ${xtensor_INCLUDE_DIR})

# Link libraries with the test executable
#target_link_libraries(test PUBLIC depixel_lib)

# Link libraries with the test_graph executable
target_link_libraries(test_graph PUBLIC depixel_lib xtensor ${OpenCV_LIBS})