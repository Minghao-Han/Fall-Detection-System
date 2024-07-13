#include <opencv2/opencv.hpp>
#include "rtmpose.hpp"
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
    // Define the box coordinates
    std::vector<DetectBox> boxs;
    boxs.push_back(DetectBox{176.636779f, 130.054153f, 405.933410f, 572.220397f,0.5f,0});
    boxs.push_back(DetectBox{418.300720f, 163.693771f, 725.852416f, 541.732482f,0.5f,0});
    boxs.push_back(DetectBox{559.033569f, 81.396133f, 815.691528f, 545.731567f,0.5f,0});

    // Create an instance of the RTMPose class
    RTMPose rtmpose("./rtmpose-s.bin", "cpu");

    // Perform pose estimation
    std::vector<std::vector<PosePoint>> poses;
    
    // Start the timer
    clock_t startTime = clock();

    // Perform pose estimation
    poses = rtmpose(image, boxs);

    // Calculate the elapsed time
    clock_t endTime = clock();
    double elapsedTime = double(endTime - startTime) / CLOCKS_PER_SEC;

    // Print the elapsed time
    std::cout << "Elapsed time: " << elapsedTime << " seconds" << std::endl;

    // Print the pose points
    for(auto man: poses){
        std::cout<<std::endl;
        for (auto posepoint : man) {
        std::cout << "Pose points: " << posepoint.x << ", " << posepoint.y << ", " << posepoint.score << std::endl;
    }
    }
    std::cout << std::endl;
}