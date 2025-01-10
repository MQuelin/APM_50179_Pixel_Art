#pragma once

#include "graph.hpp"

#include <boost/polygon/point_data.hpp>
#include <boost/polygon/segment_data.hpp>
#include <boost/polygon/voronoi.hpp>
#include <cstddef>
#include <opencv2/core/mat.hpp>
#include <set>
#include <vector>

namespace dpxl {

// Points on the voronoi grid can be represented by a unique index
// There are (4*h+1)*(4*w+1) possible points
// The standard for representing position is that each pixel is centered at
// (2*i,2*j) Such that a Voronoi point is at a position (i,j) with i,j integer
// The unique index is then (i*(4*w+1) + j)
//
// List of all cells and their corresponding node index
typedef std::vector<std::vector<size_t>> CellArray;

// Vector of all possible nodes
// at index i is represented the indices of points connected to point i
// such that valency_of_point_at_index(i) = NodeArray[].size()
typedef std::vector<std::set<size_t>> NodeArray;

class VoronoiCells {
public:
  VoronoiCells() {};

  // Build a valency-2-collapsed voronoi representation of the pixel graph
  void build_from_graph(Graph g);

  // std::pair<size_t, size_t> node_position_from_idx(size_t idx);

  CellArray &get_cells() { return m_cells; }
  NodeArray &get_nodes() { return m_nodes; }

  cv::Mat draw(size_t scale_factor, const xt::xarray<float>& img);
  cv::Mat colorCells(size_t scale_factor, const xt::xarray<float>& img);

private:
  size_t c_idx(size_t i, size_t j);
  size_t n_idx(size_t i, size_t j, size_t k, size_t l);

  std::pair<size_t, size_t> n_pos(size_t idx);

  void collapse_valency2_nodes();

  size_t m_h;
  size_t m_w;

  CellArray m_cells;
  NodeArray m_nodes;
};
} // namespace dpxl
