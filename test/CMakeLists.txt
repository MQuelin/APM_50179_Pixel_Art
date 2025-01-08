if(BUILD_TESTING)
  add_executable(unit_tests test.cpp)
  target_link_libraries(unit_tests PUBLIC depixel_lib)
  target_link_libraries(unit_tests PRIVATE GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(unit_tests)
endif()
