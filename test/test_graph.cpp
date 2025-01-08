#include "depixel_lib/graph.hpp"
#include <iostream>
#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp> 

int main()
{
    // Create a graph from an image
    auto island_graph = dpxl::Graph("/users/eleves-a/2022/leandre.simon/APM_50179_Pixel_Art/examples/2x2_test.png");

    std::cout << "YUV array \n" << island_graph.get_image() << std::endl;

    // Get neighbours
    auto neighbours = island_graph.get_neighbours();

    // Print the neighbours array
    std::cout << "Neighbour array \n" << neighbours << std::endl;

    return 0;
}
