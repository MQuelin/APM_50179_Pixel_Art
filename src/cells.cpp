#include "cells.hpp"
#include "graph.hpp"

#include <boost/polygon/voronoi_diagram.hpp>
#include <cstddef>

typedef boost::polygon::voronoi_diagram<double> VoronoiDiagram;
typedef VoronoiDiagram::vertex_type VoronoiVertex;
typedef VoronoiDiagram::cell_type VoronoiCell;
typedef VoronoiDiagram::edge_type VoronoiEdge;

namespace dpxl {
void VoronoiCells::build_from_graph(const Graph g) {

  auto neighbours = g.get_neighbours();
  auto h = neighbours.shape()[0];
  auto w = neighbours.shape()[1];

  for (size_t i; i < h; i++) {
    for (size_t j; j < w; j++) {
      m_points.push_back(Point(2 * i, 2 * j));
      for (size_t n; n < 8; n++) {
        if (neighbours.at(i, j, n)) {
          direction d = get_direction(n);
          m_segments.push_back(Segment(
              Point(2 * i, 2 * j), Point(2 * i + d.first, 2 * j + d.second)));
        }
      }
    }
  }

  // Establish Voronoi Diagram
  boost::polygon::construct_voronoi(m_points.begin(), m_points.end(),
                                    m_segments.begin(), m_segments.end(),
                                    &m_vd);

  // Traverse Voronoi Diagram vertices to create our simplified graph that
  // accepts removal of valency 2 nodes
  for (auto it = m_vd.vertices().begin(); it != m_vd.vertices().end(); it++) {
    const VoronoiVertex *vert = &(*it);
    const VoronoiEdge *edge = vert->incident_edge();
    if (!edge) {
      break;
    }
    do {
      std::cout << "Neighbours:\n" << std::endl;
      if (edge->is_primary()) {
        const VoronoiVertex *neighbor =
            edge->vertex0(); // Neighbor at the end of the edge
        if (neighbor && neighbor != vert) {
          std::cout << "    (" << neighbor->x() << ", " << neighbor->y()
                    << ")\n";
        }
      }
      edge = edge->rot_next(); // Rotate to the next incident edge
    } while (edge != vert->incident_edge());
  }
}
} // namespace dpxl
