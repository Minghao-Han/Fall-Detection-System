#ifndef _RTM_DET__HPP
#define _RTM_DET__HPP 1

#include <string>

#include "utils.hpp"
#include "hbDNNheader.h"
#include <opencv2/opencv.hpp>

class RTMDet {
public:
	RTMDet();
	virtual~RTMDet();

public:
	vector<DetectBox> RTMDet::operator()(const cv::Mat& input_mat,const float threshold);
private:
    hbDNNHandle_t dnn_handle;
    hbDNNTensor input;
    hbDNNTensorProperties input_properties;
    int model_output_count;

};

#endif // !_RTM_DET__HPP