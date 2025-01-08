#include "cells.hpp"
#include "graph.hpp"

#include <cstddef>

namespace dpxl {
void VoronoiCells::build_from_graph(const Graph g) {
  auto neighbours = g.get_neighbours();
  auto h = neighbours.shape()[0];
  auto w = neighbours.shape()[1];

  for (size_t i; i < h; i++) {
    for (size_t j; j < w; j++) {
      m_points.push_back(Point(1 + 2 * i, 1 + 2 * j));
      for (size_t n; n < 8; n++) {
        if (neighbours.at(i, j, n)) {
          direction d = get_direction(n);
          m_segments.push_back(
              Segment(Point(2 * i, 2 * j),
                      Point(1 + 2 * i + d.first, 1 + 2 * j + d.second)));
        }
      }
    }
  }

  boost::polygon::construct_voronoi(m_points.begin(), m_points.end(),
                                    m_segments.begin(), m_segments.end(),
                                    &m_vd);
}
} // namespace dpxl
