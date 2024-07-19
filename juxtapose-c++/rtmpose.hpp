#ifndef _RTMPOSE_HPP
#define _RTMPOSE_HPP 1

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <vector>
#include <string>
#include <tuple>
#include <sys/stat.h>
#include "utils.hpp"
#include "dnn/hb_dnn.h"
#include "dnn/hb_sys.h"

using namespace std;
using namespace cv;

//待确定
#define SIMCC_X_LEN 6528 //17*384
#define SIMCC_Y_LEN 8704 //17*512

class RTMPose {
public:
    RTMPose(const char* model_path, const string& device = "cpu");

    std::vector<std::vector<PosePoint>> operator()(const cv::Mat& input_mat, const vector<DetectBox>& boxs);

private:
    hbDNNHandle_t dnn_handle;
    hbDNNTensor input;
    hbDNNTensorProperties input_properties;
    hbDNNTensorProperties *output_properties;
    int model_output_count;
    Size model_input_size;
    Scalar mean;
    Scalar std;


    static const int32_t SIMCC_LEN[2];
    // string device;
    std::pair<cv::Mat, cv::Mat> CropImageByDetectBox(const cv::Mat& input_image, const DetectBox& box);

    vector<Mat> inference(const Mat& im);
    tuple<Mat, Point2f, Point2f> preprocess(const Mat& img, const Rect& bbox);
    tuple<vector<vector<Point2f>>, vector<vector<float>>> postprocess(
        const vector<Mat>& outputs,
        const Point2f& center,
        const Point2f& scale,
        float simcc_split_ratio = 2.0
    );
};

#endif // RTMPOSE_HPP
