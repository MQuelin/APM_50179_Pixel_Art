#include "depixel_lib/depixelize.hpp"
#include "cells.hpp"
#include "graph.hpp"
#include "spline.hpp"

namespace dpxl {

xt::xarray<float> depixelize(xt::xarray<float> &img_array,
                             float scaling_factor) {
  // Processing steps:
  // 1 - Establish similarity graph
  // 2 - Resolve crossings
  // 3 - Create reshaped cells
  // 4 - Define splines based on reshaped cells
  // 5 - Create new tensor with resolution scaled based on scaling

  //// img_array : [r,c,3]
  //// graph : [r,c,8] ? or object with the image array and another separate
  //// array for neighbours ?
  // xt::xarray graph = make_graph(img_array);
  // resolve_crossings(graph);
  //
  //// cells : [4r, 4c, 3]
  // xt::xarray cells = get_voronoi_cells(graph);
  //
  // Spline spline = spline_from_voronoi(cells);
  //
  // xt::xarray upscaled_img_arr =
  // spline.render(std::floor(img_array.shape()[0]*scaling_factor),
  // std::floor(img_array.shape()[1]*scaling_factor));
  //
  // return upscaled_img_arr;

  // TODO actuallly process array correctly
  return img_array;
}

} // namespace dpxl
