#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include <xtensor/xadapt.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xtensor_forward.hpp>

#include <filesystem>
#include <iostream>

#include "depixel_lib/cells.hpp"
#include "depixel_lib/depixelize.hpp"
#include "depixel_lib/graph.hpp"
#include "depixel_lib/spline.hpp"

namespace fs = std::filesystem;

namespace dpxl {

void depixelize(const std::string &image_path, bool save_image) {
  // Processing steps:
  // 1 - Establish similarity graph
  // 2 - Resolve crossings
  // 3 - Create reshaped cells

  fs::path output_dir = "visualisation";
  fs::create_directories(output_dir); // Ensure the output directory exists
  std::string file_name = fs::absolute(image_path).stem().string();

  Graph graph(image_path);
  // compute the neighbours
  graph.compute_neighbours();
  if (save_image) {
    cv::Mat neighbours_computed = graph.draw_neighbours();

    fs::path output_path = output_dir / (file_name + "_initial_neighbours.png");

    if (cv::imwrite(output_path.string(), neighbours_computed)) {
      std::cout << "Output image saved to " << output_path << std::endl;
    } else {
      std::cerr << "Failed to save the output image." << std::endl;
    }
  }

  // remove trivial edges (from flat shaded area)
  graph.remove_trivial_edges();
  if (save_image) {
    cv::Mat trivial_edges_removed = graph.draw_neighbours();

    fs::path output_path =
        output_dir / (file_name + "_trivial_edges_removed.png");

    if (cv::imwrite(output_path.string(), trivial_edges_removed)) {
      std::cout << "Output image saved to " << output_path << std::endl;
    } else {
      std::cerr << "Failed to save the output image." << std::endl;
    }
  }

  // resolve non trivial cross edges with heuristics
  graph.resolve_diagonals();
  if (save_image) {
    cv::Mat heuristics_applied = graph.draw_neighbours();

    fs::path output_path = output_dir / (file_name + "_heuristics_applied.png");

    if (cv::imwrite(output_path.string(), heuristics_applied)) {
      std::cout << "Output image saved to " << output_path << std::endl;
    } else {
      std::cerr << "Failed to save the output image." << std::endl;
    }
  }

  // created voronoi_cells
  VoronoiCells cells;
  cells.build_from_graph(graph);

  if (save_image) {
    //represent the voronoi cells
    cv::Mat voronoi_cells = cells.draw(100, graph.get_image());
  

    fs::path output_path = output_dir / (file_name + "_voronoi_cells.png");

    if (cv::imwrite(output_path.string(), voronoi_cells)) {
      std::cout << "Output image saved to " << output_path << std::endl;
    } else {
      std::cerr << "Failed to save the output image." << std::endl;
    }
  }

  cv::Mat voronoi_cells_colored = cells.colorCells(100, graph.get_image());

  fs::path output_path = output_dir / (file_name + "_voronoi_cells_colored.png");

  if (cv::imwrite(output_path.string(), voronoi_cells_colored)) {
    std::cout << "Output image saved to " << output_path << std::endl;
  } else {
    std::cerr << "Failed to save the output image." << std::endl;
  }

    // 4 - Define splines based on reshaped cells
    // 5 - Create new tensor with resolution scaled based on scaling

    //// img_array : [r,c,3]
    //// graph : [r,c,8] ? or object with the image array and another separate
    //// array for neighbours ?
    // xt::xarray graph = make_graph(img_array);
    // resolve_crossings(graph);
    //
    //// cells : [4r, 4c, 3]
    // xt::xarray cells = get_voronoi_cells(graph);
    //
    // Spline spline = spline_from_voronoi(cells);
    //
    // xt::xarray upscaled_img_arr =
    // spline.render(std::floor(img_array.shape()[0]*scaling_factor),
    // std::floor(img_array.shape()[1]*scaling_factor));
    //
    // return upscaled_img_arr;

    // TODO actuallly process array correctly

}

} // namespace dpxl

#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  // Check if enough arguments are provided
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <path_to_image> [--save_image]"
              << std::endl;
    return 1;
  }

  // Get the path to the image
  std::string relative_path = argv[1];

  // Check if the optional '--save_image' argument is provided
  bool save_image = false;
  if (argc > 2 && std::string(argv[2]) == "--save_image") {
    save_image = true;
  }

  // Call depixelize with the specified arguments
  dpxl::depixelize(relative_path, save_image);

  return 0;
}
