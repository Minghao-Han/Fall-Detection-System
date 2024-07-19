// #ifdef _Detector_CPP
// #define _Detector_CPP

#include <opencv2/opencv.hpp>
#include <queue>
#include <string>
#include <filesystem>
#include <vector>
#include <chrono>
#include <memory>
#include <iostream>
#include <string.h>
#include "./rtmpose/rtmpose.hpp"
#include "./fcos/fcos.hpp"
#include "./DetecResult.h"

// Typedefs for convenience
using namespace std;
using namespace cv;



class Detector {
public:
    Detector(queue<Results> &results_ref, const char *fcos_model_path, const char *rtmpose_model_path)
        : fcosdet(fcos_model_path), rtmpose(rtmpose_model_path), results(results_ref) {}

    // this function is designed to be used in thread pool;
    void detect_bbox_kpts(bool *detect_continue) {
        cv::Mat img;
        // @param const cv::Mat& input_mat,float threshold
        vector<DetectBox> boxes = this->fcosdet.get_bbox(img); // get_bbox是FCOSDetector类的一个方法

        // kpt detection
        vector<std::array<PosePoint,17>> kpts;
        if (!boxes.empty()) {
            // @param cv::Mat& input_mat, const vector<DetectBox>& boxes
            kpts = this->rtmpose.get_kpts(img, boxes); // 假设get_kpts是RTMPose类的一个方法
        }

        // build result 
        Results result;
        if (!boxes.empty()) {
            result.kptss = kpts;
            result.bboxes = boxes;
        } else {
            result.kptss = {};
            result.bboxes = {};
        }

        this->results.push(result);
    }

private:
    FCOSDetector fcosdet;
    RTMPose rtmpose;
    std::queue<Results> &results;
};
// #endif // _Detector_CPP