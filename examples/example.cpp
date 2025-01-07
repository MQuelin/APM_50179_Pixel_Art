#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <filesystem> // To handle folder creation

using namespace cv;

int main() {
    // Set the image path
    std::string image_path = samples::findFile("examples/TL_Creatures.png");
    Mat img = imread(image_path, IMREAD_COLOR);

    // Check if the image is successfully read
    if (img.empty()) {
        std::cerr << "Could not read the image: " << image_path << std::endl;
        return 1;
    }

    // Define the output folder and file name
    std::string output_folder = "visualisation";
    std::string output_file = output_folder + "/example_output_image.png";

    // Create the output folder if it doesn't exist
    if (!std::filesystem::exists(output_folder)) {
        std::filesystem::create_directories(output_folder);
    }

    // Save the image to the output file
    if (!imwrite(output_file, img)) {
        std::cerr << "Failed to save the image to: " << output_file << std::endl;
        return 1;
    }

    // Notify the user of the successful save
    std::cout << "Image saved successfully to: " << output_file << std::endl;

    return 0;
}
