#include "depixel_lib/graph.hpp"
#include "depixel_lib/utils.hpp"


#include <xtensor/xarray.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xio.hpp> 
#include <iostream>



namespace dpxl {

    Graph::Graph(xt::xarray<float>& img) {
        m_img = img;
        
        init_graph();
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

        init_graph();
    }

    void Graph::init_graph() {
        // Initialize m_neighbours
        std::size_t height = get_height();
        std::size_t width = get_width();
        m_neighbours = xt::xarray<bool>::from_shape({height, width, 8});
    }
        
    std::size_t Graph::get_height() {
        return m_img.shape()[0];
    }

    std::size_t Graph::get_width() {
        return m_img.shape()[1];
    } 


    void Graph::compute_neighbours() {
        std::size_t height = get_height();
        std::size_t width = get_width();

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
                        m_neighbours(i, j, k) = is_close_color(color_pixel, color_neighboor);
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

    void Graph::resolve_diagonals(){
        std::size_t height = get_height();
        std::size_t width = get_width();
        // Iterate over each pixel
        for (std::size_t i = 0; i < height - 1; ++i) {
            for (std::size_t j = 0; j < width - 1; ++j) {
                if (m_neighbours(i,j,7) && m_neighbours(i + 1,j,1)){
                    heuristics(i,j);
                }
            }
        }
    }    

    void Graph::remove_trivial_edges() {
        // this function removes the diagonal edges when all 4 corners of a square are the same colors (flat shaded region)
        std::size_t height = get_height();
        std::size_t width = get_width();
        // Iterate over each pixel
        for (std::size_t i = 0; i < height - 1; ++i) {
            for (std::size_t j = 0; j < width - 1; ++j) {
                if (m_neighbours(i,j,7) && m_neighbours(i + 1,j,1) && m_neighbours(i,j,6)) {
                    m_neighbours(i,j,7) = false;
                    m_neighbours(i + 1,j,1) = false;
                    m_neighbours(i,j + 1, 5) = false;
                    m_neighbours(i + 1, j + 1, 3) = false;
                }
            }
        }
    }


    bool Graph::is_close_color(const xt::xarray<float>& color_pixel, const xt::xarray<float>& color_neighboor) {
        // Compare Y, U, and V differences
        float delta_Y = std::abs(color_pixel(0) - color_neighboor(0)); // Y channel
        float delta_U = std::abs(color_pixel(1) - color_neighboor(1)); // U channel
        float delta_V = std::abs(color_pixel(2) - color_neighboor(2)); // V channel

        // Check if the differences are within the thresholds
        return (delta_Y <= Y_THRESHOLD && delta_U <= U_THRESHOLD && delta_V <= V_THRESHOLD);
    }


    cv::Mat Graph::draw_neighbours() {
        // Create a copy of the base image to draw on
        cv::Mat img_yuv = utils::arr_to_mat(m_img);
        cv::Mat img_bgr;
        cv::cvtColor(img_yuv, img_bgr, cv::COLOR_YUV2BGR);

        

        // Upscale the image
        cv::Mat output_image;
        cv::resize(img_bgr, output_image, cv::Size(), scale_factor, scale_factor, cv::INTER_NEAREST);

        // Draw grid lines (gray color)
        for (int y = 0; y <= output_image.rows; y += scale_factor) {
            cv::line(output_image, cv::Point(0, y), cv::Point(output_image.cols, y), cv::Scalar(192, 192, 192), 1);
        }
        for (int x = 0; x <= output_image.cols; x += scale_factor) {
            cv::line(output_image, cv::Point(x, 0), cv::Point(x, output_image.rows), cv::Scalar(192, 192, 192), 1);
        }

        //cv::imwrite("visualisation/output_base_image.png", output_image);

        // Iterate through each pixel
        int rows = m_neighbours.shape()[0];
        int cols = m_neighbours.shape()[1];

        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                // Get the 7 neighbors for the current pixel
                std::vector<bool> n = {
                    m_neighbours(y, x, 0),
                    m_neighbours(y, x, 1),
                    m_neighbours(y, x, 2),
                    m_neighbours(y, x, 3),
                    m_neighbours(y, x, 4),
                    m_neighbours(y, x, 5),
                    m_neighbours(y, x, 6)
                };

                // Define pixel coordinates for neighbors (scaled)
                std::vector<cv::Point> neighbor_coords = {
                    cv::Point((x + 1) * scale_factor, y * scale_factor),      // 0
                    cv::Point((x + 1) * scale_factor, (y - 1) * scale_factor), // 1
                    cv::Point(x * scale_factor, (y - 1) * scale_factor),       // 2
                    cv::Point((x - 1) * scale_factor, (y - 1) * scale_factor), // 3
                    cv::Point((x - 1) * scale_factor, y * scale_factor),       // 4
                    cv::Point((x - 1) * scale_factor, (y + 1) * scale_factor), // 5
                    cv::Point(x * scale_factor, (y + 1) * scale_factor)        // 6
                };

                // Current pixel center (scaled)
                cv::Point current_pixel(x * scale_factor, y * scale_factor);

                // Draw lines for active neighbors
                for (std::size_t i = 0; i < n.size(); ++i) {
                    if (n[i]) {
                        cv::Point neighbor_pixel = neighbor_coords[i];

                        // Current pixel center (scaled and shifted to the center of the pixel)
                        cv::Point current_pixel(x * scale_factor + scale_factor / 2, y * scale_factor + scale_factor / 2);

                        // Draw lines for active neighbors
                        for (std::size_t i = 0; i < n.size(); ++i) {
                            if (n[i]) {
                                // Neighbor pixel center (scaled and shifted)
                                cv::Point neighbor_pixel(
                                    neighbor_coords[i].x + scale_factor / 2,
                                    neighbor_coords[i].y + scale_factor / 2
                                );

                                // Check bounds before drawing
                                if (neighbor_pixel.x >= 0 && neighbor_pixel.x < output_image.cols &&
                                    neighbor_pixel.y >= 0 && neighbor_pixel.y < output_image.rows) {
                                    cv::line(output_image, current_pixel, neighbor_pixel, cv::Scalar(0, 0, 255), 2);
                                }
                            }
                        }
                    }
                }
            }
            //std::string file_path = "visualisation/output_image_" + std::to_string(y) + ".png";
            //cv::imwrite(file_path, output_image);
        }

        // Optionally downscale the final image
        // cv::resize(output_image, output_image, img_bgr.size(), 0, 0, cv::INTER_NEAREST);

        return output_image;
    }
} // namespace dpxl


