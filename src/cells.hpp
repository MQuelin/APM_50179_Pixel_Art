#pragma once

#include "graph.hpp"

#include <boost/polygon/point_data.hpp>
#include <boost/polygon/segment_data.hpp>
#include <boost/polygon/voronoi.hpp>
#include <cstddef>

namespace dpxl {

typedef boost::polygon::point_data<size_t> Point;
typedef boost::polygon::segment_data<size_t> Segment;

class VoronoiCells {
public:
  VoronoiCells() {};

  void build_from_graph(const Graph g);

private:
  std::vector<Point> m_points;
  std::vector<Segment> m_segments;
  boost::polygon::voronoi_diagram<double> m_vd;
};
} // namespace dpxl
