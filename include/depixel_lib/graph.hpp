#pragma once
#include <xtensor/xtensor_forward.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xarray.hpp>
#include <opencv2/opencv.hpp>



namespace dpxl {
class Graph {

public:
  Graph(xt::xarray<float> &img);
  Graph(const std::string& image_path);

  xt::xarray<float> get_image();
  xt::xarray<bool> get_neighbours();

  cv::Mat draw_neighbours();

private:
  xt::xarray<float> m_img;
  xt::xarray<bool> m_neighbours;

  
  void init_graph();
  void compute_neighbours();
  void remove_trivial_edges();

  bool is_close_color(const xt::xarray<float>& color_pixel, const xt::xarray<float>& color_neighboor);

  // Define the thresholds for each channel (normalized to [0, 1] range)
  const float Y_THRESHOLD = 48.0 / 255.0;
  const float U_THRESHOLD = 7.0 / 255.0;
  const float V_THRESHOLD = 6.0 / 255.0;

  // Scale factor for upscaling for the draw function
  const int scale_factor = 100; // Adjust as needed for better visibility
};
} // namespace dpxl
