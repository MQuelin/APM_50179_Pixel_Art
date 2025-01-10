#pragma once

/**
 * @brief takes a relative path to an image and returns the upscaled version
 * @param image_path, the relative path of the image, 
 * @param save_image (optional), to save the different steps
 */
namespace dpxl {
    void depixelize(const std::string& image_path, bool save_image=false);
}

