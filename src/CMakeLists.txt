set(LIB_SRCS graph.cpp depixelize.cpp cells.cpp spline.cpp)

find_package(xtensor REQUIRED)

add_library(depixel_lib ${LIB_SRCS})
target_include_directories(depixel_lib PUBLIC ${CMAKE_SOURCE_DIR}/include ${xtensor_INCLUDE_DIRS})
target_link_libraries(depixel_lib PUBLIC xtensor)
