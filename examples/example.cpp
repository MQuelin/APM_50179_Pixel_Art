#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include <xtensor/xadapt.hpp>
#include <xtensor/xarray.hpp>

#include "cells.hpp"
#include "graph.hpp"

#include <filesystem>
#include <iostream>
#include <xtensor/xtensor_forward.hpp>

int main() {
  // Set the image path
  cv::Mat img_base =
      imread("../examples/SpaceShip-24x24.png", cv::IMREAD_COLOR);
  cv::Mat img;
  img_base.convertTo(img, CV_32FC3, 1. / 255.);

  // Check if the image is successfully read
  if (img.empty()) {
    std::cerr << "Could not read the image: " << std::endl;
    return 1;
  }

  // Define the output folder and file name
  std::string output_folder = "visualisation";
  std::string output_file = output_folder + "/example_output_image.png";

  // Create the output folder if it doesn't exist
  if (!std::filesystem::exists(output_folder)) {
    std::filesystem::create_directories(output_folder);
  }

  // Save the image to the output file
  if (!imwrite(output_file, img_base)) {
    std::cerr << "Failed to save the image to: " << output_file << std::endl;
    return 1;
  }

  // Notify the user of the successful save
  std::cout << "Image saved successfully to: " << output_file << std::endl;
  // imshow("Display window", img);
  // cv::waitKey(0); // Wait for a keystroke in the window

  //   int type = img.type();
  //   std::cout << "image type: " << cv::typeToString(type) << std::endl;

  //   //std::cout << img << std::endl;
  //   std::cout << "mat_to_arr" << std::endl;
  //   auto img_arr = utils::mat_to_arr(img);
  //   // std::cout << img_arr << std::endl;
  //   std::cout << "arr_to_mat" << std::endl;
  //   auto img2 = utils::arr_to_mat(img_arr);
  //   //std::cout << img2 << std::endl;

  //   cv::imwrite("../examples/SpaceShip-24x24-bis.png", img2);

  //   // imshow("Display window 2", img2);
  //   // cv::waitKey(0); // Wait for a keystroke in the window

  dpxl::Graph graph("../examples/SpaceShip-24x24.png");

  dpxl::VoronoiCells cells;

  cells.build_from_graph(graph);

  // return 0;

  auto out_img = cells.draw(100);

  // Define the output folder and file name
  std::string output_file2 = output_folder + "/example_cells_noval2.png";

  // Create the output folder if it doesn't exist
  if (!std::filesystem::exists(output_folder)) {
    std::filesystem::create_directories(output_folder);
  }

  // Save the image to the output file
  if (!imwrite(output_file2, out_img)) {
    std::cerr << "Failed to save the image to: " << output_file2 << std::endl;
    return 1;
  }

  // Notify the user of the successful save
  std::cout << "Image saved successfully to: " << output_file2 << std::endl;

  return 0;
}
