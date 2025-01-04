add_executable(example example.cpp)

find_package(OpenCV REQUIRED)
find_package(xtensor REQUIRED)

# include_directories(${CMAKE_SOURCE_DIR}/include/depixel_lib ${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include/depixel_lib ${OpenCV_INCLUDE_DIRS} ${xtensor_INCLUDE_DIR})

# target_link_libraries(example PUBLIC depixel_lib ${OpenCV_LIBS})
target_link_libraries(example PUBLIC depixel_lib xtensor ${OpenCV_LIBS})
