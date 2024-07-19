#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Path to the image file
    std::string imagePath = "./football.jpeg";

    // Read the image
    cv::Mat image = cv::imread(imagePath);

    // Check if the image was successfully loaded
    if (image.empty()) {
        std::cout << "Failed to read the image!" << std::endl;
        return -1;
    }
    std::cout<<"readed image"<<std::endl;
}
