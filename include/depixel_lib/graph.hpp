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
    
    std::size_t get_height();
    std::size_t get_width();

    cv::Mat draw_neighbours();

  private:
    xt::xarray<float> m_img;
    xt::xarray<bool> m_neighbours;

    
    void init_graph();
    void compute_neighbours();
    void resolve_diagonals();
    void remove_trivial_edges();

    //defined in heuristics.cpp
    std::size_t node_valence(std::size_t i, std::size_t j);
    void heuristics(std::size_t i, std::size_t j);
    std::size_t compute_curve_length(std::size_t i, std::size_t j);
    std::vector<std::size_t> get_neighbours_list(std::size_t i, std::size_t j);
    int compute_component_size_difference(std::size_t i, std::size_t j);

    bool is_close_color(const xt::xarray<float>& color_pixel, const xt::xarray<float>& color_neighboor);

    // Define the thresholds for each channel (normalized to [0, 1] range)
    const float Y_THRESHOLD = 48.0 / 255.0;
    const float U_THRESHOLD = 7.0 / 255.0;
    const float V_THRESHOLD = 6.0 / 255.0;

    // Scale factor for upscaling for the draw function
    const int scale_factor = 100; // Adjust as needed for better visibility
  };
} // namespace dpxl
