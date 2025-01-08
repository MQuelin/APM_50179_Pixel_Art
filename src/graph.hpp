#pragma once

#include <cstddef>
#include <utility>
#include <xtensor.hpp>
#include <xtensor/xtensor_forward.hpp>

namespace dpxl {

typedef std::pair<size_t, size_t> direction;

direction get_direction(size_t idx) {
  switch (idx) {
  case 0:
    return direction(0, 1);
    break;
  case 1:
    return direction(-1, 1);
    break;
  case 2:
    return direction(-1, 0);
    break;
  case 3:
    return direction(-1, -1);
    break;
  case 4:
    return direction(0, -1);
    break;
  case 5:
    return direction(1, -1);
    break;
  case 6:
    return direction(1, 0);
    break;
  case 7:
    return direction(1, 1);
    break;
  default:
    return direction(0, 0);
    break;
  }
}

class Graph {

public:
  Graph(xt::xarray<float> &img);

  xt::xarray<bool> get_neighbours() const;

private:
  xt::xarray<float> m_img;
  xt::xarray<bool> m_neighbours;
};
} // namespace dpxl
