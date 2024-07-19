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
#include <xtensor/xsort.hpp>
#include <xtensor/xcontainer.hpp>
#include <tuple>
#include <sys/stat.h>
#include "dnn/hb_dnn.h"
#include "dnn/hb_sys.h"
#include "DetecResult.h"


#define FCOS_MAX_OUTPUT_COUNT 15
#define CV_32FC1_SIZE 4

using namespace std;
using namespace cv;

struct Detection {
    struct BBox {
        float xmin;
        float ymin;
        float xmax;
        float ymax;
    } bbox;
    float score;
    int id;
    const char* class_name;

    bool operator>(const Detection& other) const {
        return score > other.score;
    }
};

struct ScoreId {
    float score;
    int id;
};

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
    cv::Mat bgr2nv12_opencv(const cv::Mat& bgr);
    cv::Mat preprocess(const cv::Mat &img, hbDNNTensor &input_tensor);
};

#endif // FCOS_HPP


