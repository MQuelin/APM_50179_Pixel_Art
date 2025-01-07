#include <xtensor/xtensor_forward.hpp>

namespace dpxl {
class Graph {

public:
  Graph(xt::xarray<float> &img);

  xt::xarray<bool> get_neighbours();

private:
  xt::xarray<float> m_img;
  xt::xarray<bool> m_neighbours;
};
} // namespace dpxl
