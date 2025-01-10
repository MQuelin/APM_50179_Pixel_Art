# Find necessary dependencies
find_package(xtensor REQUIRED)
find_package(OpenCV REQUIRED)

# List all source files for the library
set(LIB_SRCS 
    graph.cpp 
    cells.cpp 
    spline.cpp 
    utils.cpp 
    heuristics.cpp
)

# Create the depixel_lib library
add_library(depixel_lib ${LIB_SRCS})
target_include_directories(depixel_lib PUBLIC 
    ${CMAKE_SOURCE_DIR}/include # Include directory for headers
    ${xtensor_INCLUDE_DIRS} 
    ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(depixel_lib PUBLIC xtensor ${OpenCV_LIBS})

# Add the depixelize executable
add_executable(depixelize depixelize.cpp)
target_include_directories(depixelize PUBLIC 
    ${CMAKE_SOURCE_DIR}/include # Ensure headers are included for the executable
)
# Link both the library and OpenCV explicitly to the executable
target_link_libraries(depixelize PUBLIC depixel_lib ${OpenCV_LIBS})
