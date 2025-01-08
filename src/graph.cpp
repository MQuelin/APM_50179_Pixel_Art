#include "depixel_lib/graph.hpp"
#include "depixel_lib/utils.hpp"

#include <opencv2/opencv.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xio.hpp> 
#include <iostream>



namespace dpxl {

Graph::Graph(xt::xarray<float>& img) {
    // Convert input image to YUV and store in m_img
    cv::Mat img_bgr = cv::Mat(img.shape()[0], img.shape()[1], CV_32FC3, img.data());
    cv::Mat img_yuv;
    cv::cvtColor(img_bgr, img_yuv, cv::COLOR_BGR2YUV);

    // Adapt OpenCV matrix to xtensor array
    m_img = utils::mat_to_arr(img_yuv);
 
    // Initialize m_neighbours
    std::size_t height = m_img.shape()[0];
    std::size_t width = m_img.shape()[1];
    m_neighbours = xt::xarray<bool>::from_shape({height, width, 8});
    compute_neighbours();
}


Graph::Graph(const std::string& image_path) {
    // Load the image from the file path in BGR format (OpenCV default)
    cv::Mat img_bgr = cv::imread(image_path, cv::IMREAD_COLOR);

    // Check if the image was loaded correctly
    if (img_bgr.empty()) {
        std::cerr << "Could not read the image: " << image_path << std::endl;
        return;
    }
    //std::cout << "Base image \n" << img_bgr << std::endl;


    cv::Mat img_yuv;
    cv::cvtColor(img_bgr, img_yuv, cv::COLOR_BGR2YUV);

    //std::cout << "Base image converted to YUV \n" << img_yuv << std::endl;

    // Adapt OpenCV matrix to xtensor array
    m_img = utils::mat_to_arr(img_yuv);

    //std::cout << "xtensor array \n" << m_img << std::endl;

    // Initialize m_neighbours
    std::size_t height = m_img.shape()[0];
    std::size_t width = m_img.shape()[1];
    m_neighbours = xt::xarray<bool>::from_shape({height, width, 8});
    compute_neighbours();
}
    


void Graph::compute_neighbours() {
    std::size_t height = m_img.shape()[0];
    std::size_t width = m_img.shape()[1];

    // Iterate over each pixel
    for (std::size_t i = 0; i < height; ++i) {
        for (std::size_t j = 0; j < width; ++j) {
            // Extract the Y, U, and V values of the pixel
            auto color_pixel = xt::view(m_img, i, j, xt::all());
            for (int k = 0; k < 8; ++k) {
                // Compute neighbor coordinates
                int ni = i + ((k == 1 || k == 2 || k == 3) ? -1 : (k == 5 || k == 6 || k == 7) ? 1 : 0);
                int nj = j + ((k == 0 || k == 1 || k == 7) ? 1 : (k == 3 || k == 4 || k == 5) ? -1 : 0);
                

                // Check bounds and compare colors
                if (ni >= 0 && ni < height && nj >= 0 && nj < width) {
                    // Extract the Y, U, and V values of the neighbooring pixel 
                    auto color_neighboor = xt::view(m_img, ni, nj, xt::all());
                    // Compare Y, U, and V differences
                    float delta_Y = std::abs(color_pixel(0) - color_neighboor(0)); // Y channel
                    float delta_U = std::abs(color_pixel(1) - color_neighboor(1)); // U channel
                    float delta_V = std::abs(color_pixel(2) - color_neighboor(2)); // V channel

                    // Check if the differences are within the thresholds
                    bool same_color = (delta_Y <= Y_THRESHOLD && delta_U <= U_THRESHOLD && delta_V <= V_THRESHOLD);
                    m_neighbours(i, j, k) = same_color;
                } else {
                    m_neighbours(i, j, k) = false;
                }
            }
        }
    }
}

xt::xarray<bool> Graph::get_neighbours() {
    return m_neighbours;
}

xt::xarray<float> Graph::get_image() {
    return m_img;
}

} // namespace dpxl


