#ifndef _FCOS_HPP
#define _FCOS_HPP 1

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <vector>
#include <string>
#include <xtensor/xarray.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xindex_view.hpp>
#include <tuple>
#include <xsort.hpp>
#include <xtensor/xcontainer.hpp>
#include <tuple>
#include <sys/stat.h>
#include "utils.hpp"
#include "dnn/hb_dnn.h"
#include "dnn/hb_sys.h"

#define FCOS_MAX_OUTPUT_COUNT 15
#define CV_32FC1_SIZE 4

using namespace std;
using namespace cv;

class FCOSDetector {
public:
    FCOSDetector(const char* model_path);

    vector<DetectBox> get_bbox(const cv::Mat& input_mat);

private:
    hbDNNHandle_t dnn_handle;
    hbDNNTensor input;
    hbDNNTensorProperties input_properties;
    hbDNNTensorProperties fcos_output_properties[FCOS_MAX_OUTPUT_COUNT];
    Size model_input_size;

    void inference(hbDNNTensor &input_tensor, hbDNNTensor *output_tensors);
    tuple<vector<vector<Point2f>>, vector<vector<float>>> postprocess(
        const vector<Mat>& outputs,
        const Point2f& center,
        const Point2f& scale,
        float simcc_split_ratio = 2.0
    );
    cv::Mat bgr2nv12_opencv(const cv::Mat& bgr);
    cv::Mat preprocess(const cv::Mat &img, hbDNNTensor &input_tensor);
    std::vector<xt::xarray<float>> decode(hbDNNTensor *output_tensors, float score_threshold, const cv::Size &origin_shape, int input_size);
    std::vector<xt::xarray<float>> FCOSDetector::postprocess(hbDNNTensor* model_output,
                                           const cv::Size& model_hw_shape,
                                           const cv::Size& origin_img_shape,
                                           float score_threshold,
                                           float nms_threshold);
};

#endif // FCOS_HPP


