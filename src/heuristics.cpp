#include "depixel_lib/graph.hpp"
#include <xtensor/xview.hpp>

//This file implements the functions of Graph.hpp related to heuristic resolution of crossing diagonals

namespace dpxl {
    void Graph::heuristics(std::size_t i, std::size_t j) {
        //Define weight for each of the heuristics
        int curve_weight = 0;
        int sparse_pixel_weight = 0;
        int island_weight = 0;

        //Heuristic 1 : curve lengths from each of the 2 diagonals:   
        std::size_t curve_length_1 = std::max(compute_curve_length(i, j), compute_curve_length(i+1,j+1)); // Diagonal 1 (Top-left to Bottom-right)
        std::size_t curve_length_2 = std::max(compute_curve_length(i, j+1), compute_curve_length(i+1,j)); // Diagonal 2 (Top-right to Bottom-left)

        //Positive votes for 1, negative for 2
        curve_weight = static_cast<int>(curve_length_1) - static_cast<int>(curve_length_2);

        // Heuristic 2 : Sparse Pixels Heuristic
        // Measure the size difference of connected components in an 8x8 window between diagonal 1 and diagonal 2
        sparse_pixel_weight = compute_component_size_difference(i, j);       

        // Heuristic 3 : We treat the case of Islands
        if (node_valence(i,j) == 1 || node_valence(i+1, j+1) == 1) island_weight += 5;
        else if (node_valence(i+1,j) == 1 || node_valence(i, j+1) == 1) island_weight -= 5;

        //Compute total weight
        int total_weight = curve_weight + sparse_pixel_weight + island_weight;

        if (total_weight > 0) {
            // Keep Diagonal 1 (Top-left to Bottom-right)
            m_neighbours(i + 1, j, 1) = false;
            m_neighbours(i, j + 1, 5) = false;
        }
        else if (total_weight < 0) {
            // Keep Diagonal 2 (Top-right to Bottom-left)
            m_neighbours(i, j, 7) = false;
            m_neighbours(i + 1, j + 1, 3) = false;
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
        // Computes the valence of a node.
        std::size_t valence = 0;
        for (std::size_t k=0; k<8; ++k){
            if (m_neighbours(i,j,k)) valence++;
        }
        return valence;
    }

    int Graph::compute_component_size_difference(std::size_t i, std::size_t j){
        //Computes the difference of the component attached to 1 compared to the one attached to 2.
        xt::xarray<float> color_1 = xt::view(m_img, i, j, xt::all());
        xt::xarray<float> color_2 = xt::view(m_img, i + 1, j, xt::all());
        std::size_t height = get_height();
        std::size_t width = get_width();
        int sum = 0;

        // Define the 8x8 window bounds
        std::size_t start_row = (i > 3) ? i - 3 : 0;
        std::size_t end_row = std::min(i + 4, height);
        std::size_t start_col = (j > 3) ? j - 3 : 0;
        std::size_t end_col = std::min(j + 4, width);

        for (std::size_t k = start_row; k < end_row; ++k) {
            for (std::size_t l = start_col; l < end_col; ++l) {
                xt::xarray<float> current_color = xt::view(m_img, k, l, xt::all());

                // Compare colors to calculate the sum
                if (is_close_color(current_color, color_1)) {
                    sum--; //Voting in favor of color_2, because color_1 present
                } else if (is_close_color(current_color, color_2)) {
                    sum++; //the opposite
                }
            }
        }
        return sum;
    }

    std::vector<std::size_t> Graph::get_neighbours_list(std::size_t i, std::size_t j) {
        // Computes the neighbours list of the node of coordinate (i,j)
        auto neighbour_subarray = xt::view(m_neighbours, i,j, xt::all());
        std::vector<std::size_t> neighbour_list;
        // Iterate over the subarray to find the indices of true values
        for (std::size_t k = 0; k < 8; ++k) {
            if (neighbour_subarray(k)) {
                neighbour_list.push_back(k);
            }
        }
        return neighbour_list;
    }
    
    std::size_t Graph::compute_curve_length(std::size_t i, std::size_t j) {
        // Compute the length of a curve using a priority queue

        // Check if the starting pixel has valence 2
        if (node_valence(i, j) != 2) {
            return 0;
        }

        // Offsets for neighbor traversal (anti-clockwise starting from east)
        const std::vector<std::pair<int, int>> offsets = {
            {0, 1},   // East
            {-1, 1},  // North-East
            {-1, 0},  // North
            {-1, -1}, // North-West
            {0, -1},  // West
            {1, -1},  // South-West
            {1, 0},   // South
            {1, 1}    // South-East
        };

        // Reverse offsets for bidirectional traversal
        const std::vector<std::size_t> reverse_offsets = {4, 5, 6, 7, 0, 1, 2, 3};

        // Priority queue for BFS traversal (stores {length, pixel coordinates, direction})
        using QueueElement = std::pair<std::size_t, std::pair<std::size_t, std::size_t>>;
        std::queue<QueueElement> pq;

        // Visited set to avoid revisiting pixels
        xt::xarray<bool> visited = xt::xarray<bool>::from_shape({get_height(), get_width()});
        visited.fill(false);

        // Initialize the queue with the starting pixel
        visited(i, j) = true;
        auto neighbor_list = get_neighbours_list(i, j);

        // Add neighbors to the queue
        for (std::size_t direction : neighbor_list) {
            int ni = i + offsets[direction].first;
            int nj = j + offsets[direction].second;
            if (ni >= 0 && ni < get_height() && nj >= 0 && nj < get_width()) {
                pq.push({1, {ni, nj}}); // Length starts at 1
                visited(ni, nj) = true;
            }
        }

        std::size_t curve_length = 0;

        // BFS traversal using the priority queue
        while (!pq.empty()) {
            auto [current_length, current_pixel] = pq.front();
            pq.pop();

            std::size_t ci = current_pixel.first;
            std::size_t cj = current_pixel.second;
            curve_length = std::max(curve_length, current_length);

            if (node_valence(ci, cj) != 2) {
                continue; // Stop if the pixel is not part of the curve
            }

            // Get neighbors of the current pixel
            auto current_neighbors = get_neighbours_list(ci, cj);
            for (std::size_t direction : current_neighbors) {
                int ni = ci + offsets[direction].first;
                int nj = cj + offsets[direction].second;

                // Skip already visited nodes
                if (ni >= 0 && ni < get_height() && nj >= 0 && nj < get_width() && !visited(ni, nj)) {
                    pq.push({current_length + 1, {ni, nj}});
                    visited(ni, nj) = true;
                }
            }
        }
        return curve_length;
    }
}