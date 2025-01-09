#include "depixel_lib/graph.hpp"



#include <xtensor/xarray.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xio.hpp> 
#include <iostream>



namespace dpxl {
    void Graph::heuristics(std::size_t i, std::size_t j) {
        // Step 1: Define weights for the heuristics
        int curve_weight = 0;
        int sparse_pixel_weight = 0;
        int island_weight = 0;

        // Step 2: Curves Heuristic
        // Check if the pixels are part of long curve features
        std::size_t curve_length_1 = compute_curve_length(i, j); // Diagonal 1 (Top-left to Bottom-right)
        std::size_t curve_length_2 = compute_curve_length(i, j+1); // Diagonal 2 (Top-right to Bottom-left)
        
        std::cout << "Curve1 " << curve_length_1 << std::endl;
        std::cout << "Curve2 " << curve_length_2 << std::endl;

        curve_weight = static_cast<int>(curve_length_1) - static_cast<int>(curve_length_2);

        std::cout << "Curve " << curve_weight << std::endl;

        // Step 3: Sparse Pixels Heuristic
        // Measure the size of connected components for both diagonals in an 8x8 window
        sparse_pixel_weight = compute_component_size_difference(i, j);

        std::cout << "Sparse " << sparse_pixel_weight << std::endl; 

        // Step 4: Islands Heuristic
        // Check for valence-1 nodes that would create a single disconnected pixel
        

        //We treat the case of Islands
        if (node_valence(i,j) == 1 || node_valence(i+1, j+1) == 1) island_weight += 5;
        else if (node_valence(i+1,j) == 1 || node_valence(i, j+1) == 1) island_weight -= 5;

        std::cout << "Island " << island_weight << std::endl;

        // Step 5: Determine which diagonal to keep based on weights
        int total_weight = curve_weight + sparse_pixel_weight + island_weight;

        if (total_weight > 0) {
            // Keep Diagonal 1 (Top-left to Bottom-right)
            m_neighbours(i, j, 7) = true;
            m_neighbours(i + 1, j + 1, 3) = true;
            m_neighbours(i + 1, j, 1) = false;
            m_neighbours(i, j + 1, 5) = false;
        }
        else if (total_weight < 0) {
            // Keep Diagonal 2 (Top-right to Bottom-left)
            m_neighbours(i, j, 7) = false;
            m_neighbours(i + 1, j + 1, 3) = false;
            m_neighbours(i + 1, j, 1) = true;
            m_neighbours(i, j + 1, 5) = true;
        }
        else{
            // Keep none
            m_neighbours(i, j, 7) = false;
            m_neighbours(i + 1, j + 1, 3) = false;
            m_neighbours(i + 1, j, 1) = false;
            m_neighbours(i, j + 1, 5) = false;
        }

    }


    std::size_t Graph::node_valence(std::size_t i, std::size_t j){
        std::size_t valence = 0;
        for (std::size_t k=0; k<8; ++k){
            if (m_neighbours(i,j,k)) valence++;
        }
        return valence;
    }

    int Graph::compute_component_size_difference(std::size_t i, std::size_t j){
        xt::xarray<float> color_1 = xt::view(m_img, i, j, xt::all());
        xt::xarray<float> color_2 = xt::view(m_img, i + 1, j, xt::all());
        std::size_t height = get_height();
        std::size_t width = get_width();
        int sum = 0;

        for (auto k = std::min(0, static_cast<int>(i) - 3); k < std::max(i+4, height); ++k){
            for (auto l = std::min(0, static_cast<int>(j) - 3); k < std::max(j+4, width); ++l){
                xt::xarray<float> current_color= xt::view(m_img, k, l, xt::all());
                if (is_close_color(current_color, color_1)) sum++;
                else if(is_close_color(current_color, color_2)) sum--;
            }
        }
        return sum;
    }

    std::vector<std::size_t> Graph::get_neighbours_list(std::size_t i, std::size_t j) {
        auto neighbour_subarray = xt::view(m_neighbours, i,j, xt::all());
        std::vector<std::size_t> neighbour_list;
        // Iterate over the subarray to find the indices of true values
        for (std::size_t k = 0; k < neighbour_subarray.size(); ++k) {
            if (neighbour_subarray(k)) {
                neighbour_list.push_back(k);
            }
        }
        return neighbour_list;
    }

    std::size_t Graph::compute_curve_length(std::size_t i, std::size_t j) {
        auto neighbour_list = get_neighbours_list(i,j);
        std::size_t number_neighbours = neighbour_list.size();
        std::size_t curve_length = 0;


        std::size_t height = m_neighbours.shape()[0];
        std::size_t width = m_neighbours.shape()[1];

        // Define movement offsets for neighbors (anti-clockwise, starting from east)
        std::vector<std::pair<int, int>> offsets = {
            {0, 1},   // East
            {-1, 1},  // North-East
            {-1, 0},  // North
            {-1, -1}, // North-West
            {0, -1},  // West
            {1, -1},  // South-West
            {1, 0},   // South
            {1, 1}    // South-East
        };

        std::vector<std::size_t> reverse_offsets = {4,5,6,7,0,1,2,3};


        for (std::size_t n=0; n<number_neighbours; n++) {
            std::size_t diagonal = neighbour_list[n];
            // Determine the current neighbor coordinates for the given diagonal
            int ni = i + offsets[diagonal].first;
            int nj = j + offsets[diagonal].second;

            
            // Forward traversal
            while (ni >= 0 && ni < height && nj >= 0 && nj < width) {
                std::size_t next_diagonal = 8;
                if (node_valence(ni,nj) != 2) break;
                // Check if the neighbor is valence-2 and part of the curve
                
                auto other_neighbours = get_neighbours_list(ni,nj);
                if (other_neighbours[0] != reverse_offsets[diagonal]) {
                    next_diagonal = other_neighbours[0];
                }
                else {
                    next_diagonal = other_neighbours[1];
                }
                


                // Update length and move to the next neighbor
                curve_length++;
                ni += offsets[next_diagonal].first;
                nj += offsets[next_diagonal].second;
                diagonal = next_diagonal;
            }
        }
        return curve_length;
    }
}