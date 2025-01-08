#pragma once
#include <xtensor/xtensor_forward.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xarray.hpp>



namespace dpxl {
class Graph {

public:
  Graph(xt::xarray<float> &img);
  Graph(const std::string& image_path);

  xt::xarray<float> get_image();
  xt::xarray<bool> get_neighbours();

private:
  xt::xarray<float> m_img;
  xt::xarray<bool> m_neighbours;

  void compute_neighbours();

  // Define the thresholds for each channel (normalized to [0, 1] range)
  const float Y_THRESHOLD = 48.0 / 255.0;
  const float U_THRESHOLD = 7.0 / 255.0;
  const float V_THRESHOLD = 6.0 / 255.0;
};
} // namespace dpxl
