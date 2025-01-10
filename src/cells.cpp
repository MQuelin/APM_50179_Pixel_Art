#include "depixel_lib/cells.hpp"
#include "depixel_lib/utils.hpp"

#include <algorithm>
#include <boost/polygon/voronoi_diagram.hpp>
#include <cstddef>
#include <iterator>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <ostream>
#include <utility>
#include <vector>
#include <xtensor/xtensor_forward.hpp>

namespace dpxl {

typedef boost::polygon::voronoi_diagram<double> VoronoiDiagram;
typedef VoronoiDiagram::vertex_type VoronoiVertex;
typedef VoronoiDiagram::cell_type VoronoiCell;
typedef VoronoiDiagram::edge_type VoronoiEdge;

typedef std::pair<size_t, size_t> direction;

direction get_direction(size_t n) {
  switch (n) {
  case 0:
    return direction({0, 1});
    break;
  case 1:
    return direction({-1, 0});
    break;
  case 2:
    return direction({-1, -1});
    break;
  case 3:
    return direction({-1, -1});
    break;
  case 4:
    return direction({0, -1});
    break;
  case 5:
    return direction({1, 0});
    break;
  case 6:
    return direction({1, 1});
    break;
  case 7:
    return direction({1, 1});
    break;
  default:
    return direction({0, 0});
    break;
  }
}

size_t valency(std::set<size_t> s) { return s.size(); }

size_t VoronoiCells::c_idx(size_t i, size_t j) { return m_w * i + j; };
size_t VoronoiCells::n_idx(size_t i, size_t j, size_t k, size_t l) {
  return (4 * m_w + 1) * (4 * i + k) + 4 * j + l;
};

void VoronoiCells::build_from_graph(Graph g) {

  auto neighbours = g.get_neighbours();
  auto h = neighbours.shape()[0];
  m_h = h;
  auto w = neighbours.shape()[1];
  m_w = w;

  m_cells = CellArray(h * w);
  m_nodes = NodeArray((4 * w + 1) * (4 * h + 1));

  // Create base pseudo voronoi diagram
  // This is not a true voronoi diagram, rather we apply a set of rules designed
  // to approach what is seemingly done in the paper
  // See details in our pdf document
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {

      auto &cell = m_cells[c_idx(i, j)];
      // Wether there is an edge or not,
      // The node between to horizontal pixel is allways at the midpoint

      // Diagonally if there are no edge connecting the pixels, add a
      // midpoint Else add two point to both sides of the mid (see the image
      // in the paper)

      // We place the node in trigonometric order to know which nodes are linked
      // together

      cell.push_back(n_idx(i, j, 2, 4)); // Right midpoint

      if (neighbours(i, j, 1)) {
        cell.push_back(n_idx(i, j, 1, 5));
        cell.push_back(n_idx(i, j, -1, 3));
      } else {
        if (j < w - 1 and neighbours(i, j + 1, 3)) {
          cell.push_back(n_idx(i, j, 1, 3));
        } else {
          cell.push_back(n_idx(i, j, 0, 4));
        }
      } // Top Right

      cell.push_back(n_idx(i, j, 0, 2)); // Top midpoint

      if (neighbours(i, j, 3)) {
        cell.push_back(n_idx(i, j, -1, 1));
        cell.push_back(n_idx(i, j, 1, -1));
      } else {
        if (j > 0 and neighbours(i, j - 1, 1)) {
          cell.push_back(n_idx(i, j, 1, 1));
        } else {
          cell.push_back(n_idx(i, j, 0, 0));
        }
      } // Top Left

      cell.push_back(n_idx(i, j, 2, 0)); // Left midpoint

      if (neighbours(i, j, 5)) {
        cell.push_back(n_idx(i, j, 3, -1));
        cell.push_back(n_idx(i, j, 5, 1));
      } else {
        if (j > 0 and neighbours(i, j - 1, 7)) {
          cell.push_back(n_idx(i, j, 3, 1));
        } else {
          cell.push_back(n_idx(i, j, 4, 0));
        }
      } // Bottom Left

      cell.push_back(n_idx(i, j, 4, 2)); // Bottom midpoint

      if (neighbours(i, j, 7)) {
        cell.push_back(n_idx(i, j, 5, 3));
        cell.push_back(n_idx(i, j, 3, 5));
      } else {
        if (j < w - 1 and neighbours(i, j + 1, 5)) {
          cell.push_back(n_idx(i, j, 3, 3));
        } else {
          cell.push_back(n_idx(i, j, 4, 4));
        }
      } // Bottom Right

      // We now iterate over the nodes in the cell to connect them together
      // , creating a graph
      // We assume trigonometric ordering of the nodes
      auto num_of_nodes = cell.size();
      for (int k = 0; k < num_of_nodes; k++) {
        m_nodes[cell[k]].insert(cell[(k + 1) % num_of_nodes]);
        m_nodes[cell[(k + 1) % num_of_nodes]].insert(cell[k]);
      }
    }
  }

  collapse_valency2_nodes();
}

void VoronoiCells::collapse_valency2_nodes() {
  std::vector<size_t> deleted_nodes;

  for (int k = 0; k < m_nodes.size(); k++) {
    auto &node = m_nodes[k];
    // We only collapse valency 2 nodes that aren't on the border
    if (valency(m_nodes[k]) == 2 and
        not(k % (4 * m_w + 1) == 4 * m_w or k % (4 * m_w + 1) == 0 or
            std::div(k, 4 * m_w + 1).quot == 0 or
            std::div(k, 4 * m_w + 1).quot == 4 * m_h)) {
      auto neighbour_idx_0_ptr = m_nodes[k].begin();
      auto neighbour_idx_1_ptr = std::next(m_nodes[k].begin(), 1);

      m_nodes[*neighbour_idx_0_ptr].erase(k);
      m_nodes[*neighbour_idx_0_ptr].insert(*neighbour_idx_1_ptr);
      m_nodes[*neighbour_idx_1_ptr].erase(k);
      m_nodes[*neighbour_idx_1_ptr].insert(*neighbour_idx_0_ptr);

      node.erase(*neighbour_idx_0_ptr);
      node.erase(*neighbour_idx_1_ptr);

      // Create a list of erased nodes
      // Then go through the cells again, deleting valency 2 nodes
      //
      // TODO If we knew in how many cell a node was, we could add it to the
      // vector after having deleted the node enough time its deleted from
      // deleted_nodes to make traversal shorter ?
      deleted_nodes.push_back(k);
    }
  }

  for (int k = 0; k < m_cells.size(); k++) {
    auto cell = m_cells[k];
    std::vector<size_t> new_cell;

    for (int i = 0; i < cell.size(); i++) {
      if (std::find(deleted_nodes.begin(), deleted_nodes.end(), cell[i]) ==
          deleted_nodes.end()) {
        // Node isn't in the deleted_nodes list
        // We add it to the new cell
        new_cell.push_back(cell[i]);
      }
    }

    m_cells[k] = new_cell;
  }
}

std::pair<size_t, size_t> VoronoiCells::n_pos(size_t idx) {
  return std::make_pair(idx % (4 * m_w + 1), std::ldiv(idx, 4 * m_w + 1).quot);
}

cv::Mat VoronoiCells::draw(size_t scale_factor, const xt::xarray<float>& img) {
  
  //cv::Mat img_bgr(m_h, m_w, CV_8UC3, cv::Scalar(255, 255, 255));
  // Create a copy of the base image to draw on
  cv::Mat img_yuv = utils::arr_to_mat(img);
  cv::Mat img_bgr;
  cv::cvtColor(img_yuv, img_bgr, cv::COLOR_YUV2BGR);

  // Upscale the image
  cv::Mat output_image;
  cv::resize(img_bgr, output_image, cv::Size(), 4 * scale_factor + 1,
             4 * scale_factor + 1, cv::INTER_NEAREST);

  for (int y = 0; y < m_h; ++y) {
    for (int x = 0; x < m_w; ++x) {
      // std::cout << "Accessing cell: (" << y << "," << x << ")" << std::endl;
      auto cell = m_cells[c_idx(y, x)];
      // std::cout << "size: " << cell.size();
      // std::cout << "\nDone\n";

      // for (int k = 0; k < cell.size(); k++) {
      for (int k = 0; k < cell.size(); k++) {
        auto node_1_pos = n_pos(cell[k]);
        auto node_2_pos = n_pos(cell[(k + 1) % cell.size()]);

        cv::line(output_image,
                 cv::Point(node_1_pos.second * scale_factor,
                           node_1_pos.first * scale_factor),
                 cv::Point(node_2_pos.second * scale_factor,
                           node_2_pos.first * scale_factor),
                 cv::Scalar(0, 0, 255), 2);
      }
    }
  }

  return output_image;
}

cv::Mat VoronoiCells::colorCells(size_t scale_factor, const xt::xarray<float>& img) {
    // Convert the input image to BGR format
    cv::Mat img_yuv = utils::arr_to_mat(img);
    cv::Mat img_bgr;
    cv::cvtColor(img_yuv, img_bgr, cv::COLOR_YUV2BGR);

    // Upscale the image
    cv::Mat output_image;
    cv::resize(img_bgr, output_image, cv::Size(), 4 * scale_factor + 1,
               4 * scale_factor + 1, cv::INTER_NEAREST);

    for (int y = 0; y < m_h; ++y) {
        for (int x = 0; x < m_w; ++x) {
            // Get the color of the pixel at (y, x)
            cv::Vec3b pixel_color = img_bgr.at<cv::Vec3b>(y, x);

            // Access the cell corresponding to the pixel
            auto cell = m_cells[c_idx(y, x)];

            // Create a vector of points to define the polygon
            std::vector<cv::Point> polygon;
            for (int k = 0; k < cell.size(); k++) {
                auto node_pos = n_pos(cell[k]);
                polygon.emplace_back(
                    node_pos.first * scale_factor, // x-coordinate
                    node_pos.second * scale_factor   // y-coordinate
                );
            }

            // Fill the polygon with the pixel color
            cv::fillPoly(output_image, std::vector<std::vector<cv::Point>>{polygon},
                         cv::Scalar(pixel_color[0], pixel_color[1], pixel_color[2]));
        }
    }

    return output_image;
}



} // namespace dpxl
