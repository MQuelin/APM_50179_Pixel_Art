#include <gtest/gtest.h>

namespace {
int setup_test_func_1() { return 0; }

int setup_test_func_2() { return 1; }
} // namespace

TEST(TestModuleSetupTopic, DummyGoodTest) { EXPECT_EQ(setup_test_func_1(), 0); }

// TEST(TestModuleSetupTopic, DummyBadTest) {
//  EXPECT_EQ(setup_test_func_1(), setup_test_func_2())
//      << "Forced error successfully detected ! This test is here to check that
//      "
//         "ctest raises errors correctly.";
// }
