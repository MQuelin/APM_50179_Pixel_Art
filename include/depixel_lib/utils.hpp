#pragma once
#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include <xtensor/xadapt.hpp>
#include <xtensor/xarray.hpp>


namespace dpxl
{

namespace utils
{

    xt::xarray<float> mat_to_arr(const cv::Mat &mat);

    cv::Mat arr_to_mat(const xt::xarray<float> &arr);
};
    
}