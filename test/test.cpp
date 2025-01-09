#include <gtest/gtest.h>

#include <xtensor/xarray.hpp>
#include <xtensor/xtensor_forward.hpp>

#include "depixel_lib/cells.hpp"
#include "depixel_lib/graph.hpp"

namespace {
int setup_test_func_1() { return 0; }

int setup_test_func_2() { return 1; }

void test_voronoi_1() {

  xt::xarray<float> img = {
      {{1., 1., 1.}, {1., 1., 1.}, {1., 1., 1.}, {1., 1., 1.}},
      {{1., 1., 1.}, {1., 1., 1.}, {1., 1., 1.}, {1., 1., 1.}},
      {{0., 0., 0.}, {0., 0., 0.}, {0., 0., 0.}, {0., 0., 0.}},
      {{0., 0., 0.}, {0., 0., 0.}, {0., 0., 0.}, {0., 0., 0.}}};

  dpxl::Graph g(img);

  dpxl::VoronoiCells c;

  c.build_from_graph(g);
}
} // namespace

TEST(TestModuleSetupTopic, DummyGoodTest) { EXPECT_EQ(setup_test_func_1(), 0); }

TEST(VornoiTests, InstatiationTest) { EXPECT_NO_THROW(test_voronoi_1()); }

// TEST(TestModuleSetupTopic, DummyBadTest) {
//  EXPECT_EQ(setup_test_func_1(), setup_test_func_2())
//      << "Forced error successfully detected ! This test is here to check that
//      "
//         "ctest raises errors correctly.";
// }
