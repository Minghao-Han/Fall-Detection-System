#include "./fcos.hpp"
#include <stdint.h>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <xtensor.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xindex_view.hpp>
#include <tuple>
#include <xtensor/xcontainer.hpp>


FCOSDetector::FCOSDetector(const char *model_path)
{
    struct stat buffer;
    if (stat(model_path, &buffer) != 0) {
        throw std::runtime_error("Model path does not exist.");
    }
    
    // 第一步加载模型
    hbPackedDNNHandle_t packed_dnn_handle;
    // const char* model_file_name= "./mobilenetv1.bin";
    hbDNNInitializeFromFiles(&packed_dnn_handle, &model_path, 1);

    // 第二步获取模型名称
    const char **model_name_list;
    int model_count = 0;
    hbDNNGetModelNameList(&model_name_list, &model_count, packed_dnn_handle);

    // 第三步获取dnn_handle
    hbDNNGetModelHandle(&(this->dnn_handle), packed_dnn_handle, model_name_list[0]);

    // 第四步准备输入数据
    hbDNNGetInputTensorProperties(&(input_properties), dnn_handle, 0);
    // 第五步准备模型输出数据的空间
    // hbDNNGetOutputCount(&model_output_count, dnn_handle);
    for (int i = 0; i < FCOS_MAX_OUTPUT_COUNT; i++)
    {
        hbDNNGetOutputTensorProperties(fcos_output_properties+i, dnn_handle, i);
    }
    
}



//-------------------------------------------------inference-------------------------------------------------------------------------------------------------------
inline void FCOSDetector::inference(hbDNNTensor &input_tensor, hbDNNTensor *output_tensors){
    // 推理模型
    hbDNNTaskHandle_t task_handle = nullptr;
    hbDNNInferCtrlParam infer_ctrl_param;
    HB_DNN_INITIALIZE_INFER_CTRL_PARAM(&infer_ctrl_param);
    hbDNNInfer(&task_handle,&output_tensors,&input_tensor,dnn_handle,&infer_ctrl_param);

    // 第七步等待任务结束
    hbDNNWaitTaskDone(task_handle, 0);
    hbDNNReleaseTask(task_handle);
}


//-------------------------------------------------pre processes-------------------------------------------------------------------------------------------------------

cv::Mat FCOSDetector::bgr2nv12_opencv(const cv::Mat &image)
{
    int height = image.rows;
    int width = image.cols;
    int area = height * width;

    // Convert BGR to YUV I420
    cv::Mat yuv420p;
    cv::cvtColor(image, yuv420p, cv::COLOR_BGR2YUV_I420);

    // Separate the Y and UV planes
    cv::Mat y = yuv420p(cv::Rect(0, 0, width, height));
    cv::Mat u = yuv420p(cv::Rect(0, height, width / 2, height / 2));
    cv::Mat v = yuv420p(cv::Rect(width / 2, height, width / 2, height / 2));

    // Create the NV12 image
    cv::Mat nv12(height + height / 2, width, CV_8UC1);
    
    // Copy Y plane
    y.copyTo(nv12(cv::Rect(0, 0, width, height)));

    // Interleave U and V planes into NV12 UV plane
    for (int i = 0; i < height / 2; ++i) {
        for (int j = 0; j < width / 2; ++j) {
            nv12.at<uchar>(height + i, j * 2) = u.at<uchar>(i, j);
            nv12.at<uchar>(height + i, j * 2 + 1) = v.at<uchar>(i, j);
        }
    }

    return nv12;
}

// convert the image to NV12 format and feed it to the input tensor
cv::Mat FCOSDetector::preprocess(const cv::Mat &img, hbDNNTensor &input_tensor)
{
    cv::Mat resizedImg;
    cv::resize(img, resizedImg, cv::Size(512, 512),0.0,0.0,cv::INTER_AREA);

    // Convert the resized image to NV12 format
    return bgr2nv12_opencv(resizedImg);
}




//-------------------------------------------------post processes-------------------------------------------------------------------------------------------------------

// 解码函数，将模型输出的特征图cls bbox le 解码为检测框和置信度
void decode(hbDNNTensor* tensors,int origin_height,int origin_width,int resized_height,int resized_width, std::vector<Detection>& dets, float score_hold) {

    int strides[]={8,16,32,64,128};

    // 宽高比例缩放因子 scale_back_width 和 scale_back_height, 用于将预测的边界框调整回原始图像的尺度
    float scale_back_width = static_cast<float>(origin_width) / resized_width;
    float scale_back_height = static_cast<float>(origin_height) / resized_height;

    for (int i = 0; i < 5; i++) {

        int height = tensors[i].properties.alignedShape.dimensionSize[1];
        int width = tensors[i].properties.alignedShape.dimensionSize[2];
        int cls_channel = tensors[i].properties.alignedShape.dimensionSize[3];
        // cls bbox 和ce的第一维都是1，因为是单张图片。第二三维相同，因为是同一个特征图的高宽
        auto cls_data = xt::adapt(reinterpret_cast<float*>(tensors[i].sysMem[0].virAddr),
                                  {height,width,cls_channel});

        // 第三维是四个点
        auto bbox_data = xt::adapt(reinterpret_cast<float*>(tensors[i + 5].sysMem[0].virAddr),
                                   {height,width,4});

        auto ce_data = xt::adapt(reinterpret_cast<float*>(tensors[i + 10].sysMem[0].virAddr),
                                 {tensors[i + 10].properties.alignedShape.dimensionSize[1],
                                  tensors[i + 10].properties.alignedShape.dimensionSize[2]});



        for (int h_idx = 0; h_idx < height; ++h_idx) {
            for (int w_idx = 0; w_idx < width; ++w_idx) {
                int ce_offset = h_idx * width + w_idx;
                // 计算中心度的置信度分数，通过 Sigmoid 函数将中心度值转换为置信度
                ce_data(ce_offset) = 1.0f / (1.0f + std::exp(-ce_data(ce_offset)));

                auto cls_slice = xt::view(cls_data, h_idx, w_idx, xt::all());
                auto max_idx = xt::argmax(cls_slice)();
                float max_score = cls_slice(max_idx);

                max_score = 1.0f / (1.0f + std::exp(-max_score));
                float final_score = std::sqrt(max_score * ce_data(ce_offset));
                if (final_score <= score_hold) {
                    continue;
                }

                Detection detection;
                int stride = strides[i];
                // 在特征图中，每个像素点通常代表一个“cell”或“grid cell”，这些cell在原始图像中覆盖一个较大的区域。w_idx 和 h_idx 表示特征图上某个cell的左上角坐标，而 w_idx + 0.5 和 h_idx + 0.5 则表示该cell的中心点坐标。
                // 如果特征图尺寸为H,W 原始图尺寸为H_O,W_O 则特征图上的点对应原始图上的点为 (h_idx + 0.5) * stride
                float center_x = (w_idx + 0.5f) * stride;
                float center_y = (h_idx + 0.5f) * stride;
                //将特征图上的xyxy缩放回原始图像尺寸上的xyxy
                if(max_idx==0){
                    detection.bbox.xmin = (center_x - bbox_data(h_idx, w_idx, 0)) * scale_back_width;
                    detection.bbox.ymin = (center_y - bbox_data(h_idx, w_idx, 1)) * scale_back_height;
                    detection.bbox.xmax = (center_x + bbox_data(h_idx, w_idx, 2)) * scale_back_width;
                    detection.bbox.ymax = (center_y + bbox_data(h_idx, w_idx, 3)) * scale_back_height;
                    detection.score = final_score;
                    detection.id = max_idx;
                    dets.push_back(detection);
                }
            }
        }
    }
}
void nms(std::vector<Detection> &input, float iou_threshold, int top_k, std::vector<Detection> &result, bool suppress) {
    // 按得分降序排序
    std::stable_sort(input.begin(), input.end(), std::greater<Detection>());

    std::vector<bool> skip(input.size(), false);

    // 预计算检测框面积
    xt::xarray<float> areas = xt::zeros<float>({input.size()});
    for (size_t i = 0; i < input.size(); i++) {
        float width = input[i].bbox.xmax - input[i].bbox.xmin;
        float height = input[i].bbox.ymax - input[i].bbox.ymin;
        areas(i) = width * height;
    }

    int count = 0;
    for (size_t i = 0; count < top_k && i < skip.size(); i++) {
        if (skip[i]) {
            continue;
        }
        skip[i] = true;
        ++count;

        for (size_t j = i + 1; j < skip.size(); ++j) {
            if (skip[j]) {
                continue;
            }
            if (!suppress && input[i].id != input[j].id) {
                continue;
            }

            // 计算交集面积
            float xx1 = std::max(input[i].bbox.xmin, input[j].bbox.xmin);
            float yy1 = std::max(input[i].bbox.ymin, input[j].bbox.ymin);
            float xx2 = std::min(input[i].bbox.xmax, input[j].bbox.xmax);
            float yy2 = std::min(input[i].bbox.ymax, input[j].bbox.ymax);

            if (xx2 > xx1 && yy2 > yy1) {
                float area_intersection = (xx2 - xx1) * (yy2 - yy1);
                float iou_ratio = area_intersection / (areas(j) + areas(i) - area_intersection);
                if (iou_ratio > iou_threshold) {
                    skip[j] = true;
                }
            }
        }
        result.push_back(input[i]);
    }
}
//-------------------------------------------------whole-------------------------------------------------------------------------------------------------------
vector<DetectBox> FCOSDetector::get_bbox(const cv::Mat &input_mat)
{
    std::vector<DetectBox> detect_boxes;
    cv::Mat nv12=preprocess(input_mat, input);
    // 准备输入
    hbDNNTensor input_tensor;
    input_tensor.properties = input_properties;
    input_tensor.properties.alignedShape = input_tensor.properties.validShape;
    // Set nv12 to the input tensor data
    std::size_t input_len= nv12.total() * nv12.elemSize();
    auto &mem = input_tensor.sysMem[0];
    hbSysAllocCachedMem(&mem, input_len);
    memcpy(mem.virAddr, nv12.data, input_len);
    hbSysFlushMem(&mem, HB_SYS_MEM_CACHE_CLEAN);
    // 准备输出
    hbDNNTensor *output_tensors = new hbDNNTensor[FCOS_MAX_OUTPUT_COUNT];
    for (int i = 0; i < FCOS_MAX_OUTPUT_COUNT; i++) {
        output_tensors[i].properties = fcos_output_properties[i];
        hbSysMem &mem = output_tensors[i].sysMem[0];
        hbSysAllocCachedMem(&mem, output_tensors[i].properties.alignedByteSize);
    }
    inference(input_tensor, output_tensors);
    std::vector<Detection> dets;
    decode(output_tensors, input_mat.rows, input_mat.cols, 512, 512,dets, 0.5);
    float iou_threshold=0.6;
    int top_k=std::min((int)dets.size(),100);
    std::vector<Detection> results;
    bool suppress =true;
    nms(dets, iou_threshold, top_k, results,suppress);
    for (auto result:results) {
        DetectBox detect_box;
        detect_box.top = result.bbox.ymin;
        detect_box.left = result.bbox.xmin;
        detect_box.bottom = result.bbox.ymax;
        detect_box.right = result.bbox.xmax;
        detect_box.score = result.score;
        detect_box.label = result.id;
        detect_boxes.push_back(detect_box);
    }
    return detect_boxes;
}
