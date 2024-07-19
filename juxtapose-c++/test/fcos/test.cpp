#include <opencv2/opencv.hpp>
#include "fcos.hpp"
#include "utils.hpp"

int main() {
    FCOSDetector detector = FCOSDetector("/home/sunrise/fcos_tutorial-master/inference/fcos_512x512_nv12.bin");
    // TODO: Add your test code here
    cv::Mat img = cv::imread("/home/sunrise/fcos_tutorial-master/inference/kite.jpg");
    std::vector<DetectBox> detected = detector(img);


    return 0;
}