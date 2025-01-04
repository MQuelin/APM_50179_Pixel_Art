#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include <xtensor/xadapt.hpp>
#include <xtensor/xarray.hpp>

#include <filesystem>
#include <iostream>
#include <xtensor/xtensor_forward.hpp>

// Utilities to convert back and forth between opencv's mat and xtensor's xarray
// for use with depixel_lib
xt::xarray<float> mat_to_arr(const cv::Mat &mat) {
  int ndims = mat.dims;
  assert(ndims == 2);
  int nrows = mat.rows;
  int ncols = mat.cols;
  int nchannels = 3;
  xt::xarray<float> arr = xt::empty<float>({nrows, ncols, nchannels});
  for (int rr = 0; rr < nrows; rr++) {
    for (int cc = 0; cc < ncols; cc++) {
      cv::Vec3f pxl = mat.at<float>(rr, cc);
      cv::Vec3f pxl_copy = pxl;
      // std::cout << pxl[0] << std::endl;
      for (int chan = 0; chan < nchannels; chan++) {
        arr(rr, cc, chan) = pxl_copy[chan];
      }
    }
  }
  return arr;
}

cv::Mat arr_to_mat(const xt::xarray<float> &arr) {
  int ndims = arr.dimension();
  assert(ndims == 3 && "can only convert 3d xarrays");
  int nrows = arr.shape()[0];
  int ncols = arr.shape()[1];
  cv::Mat mat(nrows, ncols, 3, CV_32FC3);
  for (int rr = 0; rr < nrows; rr++) {
    for (int cc = 0; cc < ncols; cc++) {
      std::cout << arr(rr, cc, 0) << std::endl;
      std::cout << arr(rr, cc, 1) << std::endl;
      std::cout << arr(rr, cc, 2) << std::endl << std::endl;
      for (int chan = 0; chan < 3; chan++) {
        mat.at<cv::Vec3f>(rr, cc)[chan] = 1.; // arr(rr, cc, chan);
      };
    }
  }
  return mat;
}

int main() {
  cv::Mat img_base =
      imread("../examples/SpaceShip-24x24.png", cv::IMREAD_COLOR);
  cv::Mat img;
  img_base.convertTo(img, CV_32FC3, 1. / 255.);

  if (img.empty()) {
    std::cout << "Current directory: " << std::filesystem::current_path()
              << std::endl;
    std::cout << "Could not read the image: " << std::endl;
    return 1;
  }

  imshow("Display window", img);
  cv::waitKey(0); // Wait for a keystroke in the window

  int type = img.type();
  std::cout << "image type: " << cv::typeToString(type) << std::endl;

  std::cout << "mat_to_arr" << std::endl;
  auto img_arr = mat_to_arr(img);
  // std::cout << img_arr << std::endl;
  std::cout << "arr_to_mat" << std::endl;
  auto img2 = arr_to_mat(img_arr);
  std::cout << img2 << std::endl;

  cv::imwrite("../examples/SpaceShip-24x24-bis.png", img2);

  imshow("Display window 2", img2);
  cv::waitKey(0); // Wait for a keystroke in the window

  return 0;
}
