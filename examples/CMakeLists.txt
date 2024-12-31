add_executable(example example.cpp)
# include_directories(${CMAKE_SOURCE_DIR}/include/depixel_lib ${OpenCV_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

# target_link_libraries(example PUBLIC depixel_lib ${OpenCV_LIBS})
target_link_libraries(example PUBLIC ${OpenCV_LIBS})
