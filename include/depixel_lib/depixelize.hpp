#include <xtensor/xarray.hpp>

/**
 * @brief Takes a YUV image array and return an upscaled smooth version
 * @param img_array An image array containing YUV data, expected shape is
 * {nrows, ncols, 3}
 * @param scaling_factor The scaling factor of the upscaled image. If input
 * image is 32x32 and scaling_factor is 10. the output image will be 320x320
 */
xt::xarray<float> depixelize(xt::xarray<float> &a, float scaling);
