#include "depixel_lib/cells.hpp"

#include <boost/polygon/voronoi_diagram.hpp>
#include <cstddef>
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

void VoronoiCells::build_from_graph(const Graph g) {

  auto neighbours = g.get_neighbours();
  auto h = neighbours.shape()[0];
  auto w = neighbours.shape()[1];

  m_cells = CellArray(h * w);
  m_nodes = NodeArray((4 * w + 1) * (4 * h + 1));

  // Cell index
  auto c_idx = [w](size_t i, size_t j) { return w * i + j; };
  // Node index
  auto n_idx = [w](size_t i, size_t j, size_t k, size_t l) {
    return (4 * w + 1) * (4 * i + k) + 4 * j + l;
  };

  // Create base pseudo voronoi diagram
  // This is not a true voronoi diagram, rather we apply a set of rules designed
  // to approach what is seemingly done in the paper
  // See details in our pdf document
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {

      auto cell = m_cells[c_idx(i, j)];
      // Wether there is an edge or not,
      // The node between to horizontal pixel is allways at the midpoint

      // Diagonally if there are no edge connecting the pixels, add a
      // midpoint Else add two point to both sides of the mid (see the image
      // in the paper)

      // We place the node in trigonometric order to know which nodes are linked
      // together

      cell.push_back(n_idx(i, j, 2, 5)); // Right midpoint

      if (neighbours(i, j, 1)) {
        cell.push_back(n_idx(i, j, 1, 6));
        cell.push_back(n_idx(i, j, -1, 4));
      } else {
        if (j < w - 1 and neighbours(i, j + 1, 3)) {
          cell.push_back(n_idx(i, j, 1, 4));
        } else {
          cell.push_back(n_idx(i, j, 0, 5));
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
        cell.push_back(n_idx(i, j, 4, -1));
        cell.push_back(n_idx(i, j, 6, 1));
      } else {
        if (j > 0 and neighbours(i, j - 1, 7)) {
          cell.push_back(n_idx(i, j, 4, 1));
        } else {
          cell.push_back(n_idx(i, j, 5, 0));
        }
      } // Bottom Left

      cell.push_back(n_idx(i, j, 5, 2)); // Bottom midpoint

      if (neighbours(i, j, 7)) {
        cell.push_back(n_idx(i, j, 6, 4));
        cell.push_back(n_idx(i, j, 4, 6));
      } else {
        if (j < w - 1 and neighbours(i, j + 1, 5)) {
          cell.push_back(n_idx(i, j, 4, 4));
        } else {
          cell.push_back(n_idx(i, j, 5, 5));
        }
      } // Bottom Left

      // We now iterate over the nodes in the cell to connect them together
      // , creating a graph
      // We assume trigonometric ordering of the nodes
      auto num_of_nodes = cell.size();
      for (int k; k < num_of_nodes; k++) {
        m_nodes[cell[k]].insert(cell[(k + 1) % num_of_nodes]);
        m_nodes[cell[(k + 1) % num_of_nodes]].insert(cell[k]);
      }
    }
  }

  collapse_valency2_nodes();
}

void VoronoiCells::collapse_valency2_nodes() {
  for (int k = 0; k < m_nodes.size(); k++) {
    auto node = m_nodes[k];
    if (valency(m_nodes[k]) == 2) {
      auto neighbour_idx_0_ptr = m_nodes[k].begin();
      auto neighbour_idx_1_ptr = std::next(m_nodes[k].begin(), 1);

      m_nodes[*neighbour_idx_0_ptr].erase(k);
      m_nodes[*neighbour_idx_0_ptr].insert(*neighbour_idx_1_ptr);
      m_nodes[*neighbour_idx_1_ptr].erase(k);
      m_nodes[*neighbour_idx_1_ptr].insert(*neighbour_idx_0_ptr);

      node.erase(*neighbour_idx_0_ptr);
      node.erase(*neighbour_idx_1_ptr);
    }
  }
}

} // namespace dpxl
