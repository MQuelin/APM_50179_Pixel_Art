# Find required packages
find_package(OpenCV REQUIRED)
find_package(xtensor REQUIRED)

if(BUILD_TESTING)
  add_executable(unit_tests test.cpp)
  add_executable(test_graph test_graph.cpp)
  target_link_libraries(unit_tests PUBLIC depixel_lib)
  target_link_libraries(unit_tests PRIVATE GTest::gtest_main)
  target_link_libraries(test_graph PUBLIC depixel_lib xtensor ${OpenCV_LIBS})

  include(GoogleTest)
  gtest_discover_tests(unit_tests)
endif()