#pragma once

#include "graph.hpp"

#include <boost/polygon/point_data.hpp>
#include <boost/polygon/segment_data.hpp>
#include <boost/polygon/voronoi.hpp>
#include <cstddef>
#include <utility>
#include <vector>

namespace dpxl {

typedef boost::polygon::point_data<size_t> Point;
typedef boost::polygon::segment_data<size_t> Segment;
typedef std::pair<size_t, std::vector<size_t>> Cell;

struct Node {
  size_t m_index;
  std::vector<size_t> neighbours;

  size_t valency() const;
};

class VoronoiCells {
public:
  VoronoiCells() {};

  void build_from_graph(const Graph g);

  void collapse_valency2_nodes();

private:
  void collapse_valency2_node_at(size_t index);

  std::vector<Point> m_points;
  std::vector<Segment> m_segments;
  boost::polygon::voronoi_diagram<double> m_vd;
  std::vector<Cell> m_cells;
  std::vector<Node> m_verticies;
};
} // namespace dpxl
