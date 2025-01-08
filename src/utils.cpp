#include "depixel_lib/utils.hpp"

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
    // Utilities to convert back and forth between opencv's mat and xtensor's xarray
    // for use with depixel_lib
    xt::xarray<float> mat_to_arr(const cv::Mat &mat) {
    // Ensure the input image has 3 channels
    assert(mat.type() == CV_8UC3 && "Expected CV_8UC3 Mat");

    // Get image dimensions
    int nrows = mat.rows;
    int ncols = mat.cols;
    int nchannels = 3; // Assuming 3 channels for RGB/YUV images

    // Create an xt::xarray with the appropriate shape
    xt::xarray<float> arr = xt::empty<float>({nrows, ncols, nchannels});

    // Populate the xtensor array from the cv::Mat
    for (int rr = 0; rr < nrows; rr++) {
        for (int cc = 0; cc < ncols; cc++) {
            // Access the pixel as a 3-channel vector (uchar per channel)
            cv::Vec3b pxl = mat.at<cv::Vec3b>(rr, cc);
            for (int chan = 0; chan < nchannels; chan++) {
                arr(rr, cc, chan) = pxl[chan] / 255.0f;
            }
        }
    }
    return arr;
}


    cv::Mat arr_to_mat(const xt::xarray<float> &arr) {
    // Ensure the input array has 3 dimensions
    assert(arr.dimension() == 3 && "Expected a 3D xarray");

    // Get array dimensions
    int nrows = arr.shape()[0];
    int ncols = arr.shape()[1];
    int nchannels = arr.shape()[2];
    assert(nchannels == 3 && "Expected 3 channels");

    // Create a cv::Mat with 3 channels
    cv::Mat mat(nrows, ncols, CV_8UC3);

    // Populate the cv::Mat from the xtensor array
    for (int rr = 0; rr < nrows; rr++) {
        for (int cc = 0; cc < ncols; cc++) {
            cv::Vec3b pxl;
            for (int chan = 0; chan < nchannels; chan++) {
                pxl[chan] = static_cast<uint8_t>(arr(rr, cc, chan) * 255.0f);
            }
            mat.at<cv::Vec3b>(rr, cc) = pxl;
        }
    }
    return mat;
}

  }
}