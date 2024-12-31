set(LIB_SRCS depixelize.cpp)

add_library(depixel_lib ${LIB_SRCS})
target_include_directories(depixel_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)
