#include "fcos.hpp"
#include <stdint.h>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <xtensor/xarray.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xindex_view.hpp>
#include <tuple>
#include <xsort.hpp>
#include <xtensor/xcontainer.hpp>
#include "postprocess.cpp"


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
    xt::xarray<float> pred_bboxes=postprocess(output_tensors, input_mat.size(), input_mat.size(), 0.5, 0.6);
    for (std::size_t i = 0; i < pred_bboxes.shape()[0]; ++i) {
        DetectBox detect_box;
        detect_box.top = pred_bboxes(i, 1);
        detect_box.left = pred_bboxes(i, 0);
        detect_box.bottom = pred_bboxes(i, 3);
        detect_box.right = pred_bboxes(i, 2);
        detect_box.score = pred_bboxes(i, 4);
        detect_boxes.push_back(detect_box);
    }
    return detect_boxes;
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

    // Convert BGR to YUV I420
    printf("converting bgr to yuv420p\n");
    cv::Mat yuv420p;
    cv::cvtColor(image, yuv420p, cv::COLOR_BGR2YUV_I420);

    // Separate the Y and UV planes
    printf("separating y and uv\n");
    cv::Mat y = yuv420p(cv::Rect(0, 0, width, height));
    cv::Mat u = yuv420p(cv::Rect(0, height, width / 2, height / 2));
    cv::Mat v = yuv420p(cv::Rect(width / 2, height, width / 2, height / 2));

    // Create the NV12 image
    printf("creating nv12\n");
    cv::Mat nv12(height + height / 2, width, CV_8UC1);
    
    // Copy Y plane
    printf("copying y plane\n");
    y.copyTo(nv12(cv::Rect(0, 0, width, height)));

    // Interleave U and V planes into NV12 UV plane
    printf("interleaving uv plane\n");
    for (int i = 0; i < height / 2; ++i) {
        for (int j = 0; j < width / 2; ++j) {
            nv12.at<uchar>(height + i, j * 2) = u.at<uchar>(i, j);
            nv12.at<uchar>(height + i, j * 2 + 1) = v.at<uchar>(i, j);
        }
    }

    return nv12;
}

cv::Mat FCOSDetector::preprocess(const cv::Mat &img, hbDNNTensor &input_tensor)
{
    cv::Mat resized_img;
    cv::resize(img, resized_img, cv::Size(512, 512), 0.0, 0.0, cv::INTER_AREA);

    // Convert the resized image to NV12 format
    return bgr2nv12_opencv(resized_img);
}



//-------------------------------------------------post processes-------------------------------------------------------------------------------------------------------

inline xt::xarray<double> adjust_bbox(const xt::xarray<double>& bbox, double stride, const std::array<int, 2>& origin_shape, double input_size) {
    int h = bbox.shape()[1];
    int w = bbox.shape()[2];
    auto xyv = xt::meshgrid(xt::arange(h), xt::arange(w));
    xt::xarray<float> stack_xy = xt::stack(std::tuple(std::get<0>(xyv), std::get<1>(xyv)), 2) + 0.5;
    stack_xy *= stride;
    // adjust bbox
    auto x1 = xt::view(stack_xy, xt::all(), 0) - xt::view(stack_xy, xt::all(), 0);
    auto y1 = xt::view(stack_xy, xt::all(), 1) - xt::view(stack_xy, xt::all(), 1);
    auto x2 = xt::view(stack_xy, xt::all(), 0) + xt::view(stack_xy, xt::all(), 2);
    auto y2 = xt::view(stack_xy, xt::all(), 1) + xt::view(stack_xy, xt::all(), 3);
    xt::xarray<float> adjusted_bbox = xt::stack(xt::xtuple(x1, y1, x2, y2), -1);

    float scale_w = static_cast<float>(origin_shape[1]) / input_size;
    float scale_h = static_cast<float>(origin_shape[0]) / input_size;
    xt::xarray<float> scale = {scale_w, scale_h, scale_w, scale_h};
    adjusted_bbox *= scale;
    return adjusted_bbox;
}



std::vector<xt::xarray<float>> FCOSDetector::decode(hbDNNTensor *output_tensors, float score_threshold, const cv::Size &origin_shape, int input_size)
{
    std::vector<std::tuple<cv::Rect2f, float, int>> bboxes;
    std::vector<uint8_t> strides = {8, 16, 32, 64, 128};
    // 将筛选后的结果加入到 bboxes
    std::vector<xt::xarray<float>> results;
    for (uint8_t i = 0; i < static_cast<uint8_t>(strides.size()); ++i) {
        // int8_t cls_idx= i;
        uint8_t bbox_idx= i + 5;
        uint8_t ce_idx= i + 10;

        // 获取张量的形状
        int cls_shape[4];
        int bbox_shape[4];
        int ce_shape[4];
        //cls ce bbox的特征图的高宽是一样的

        int *shape = output_tensors[i].properties.alignedShape.dimensionSize;
        cls_shape[0]=bbox_shape[0]=ce_shape[0] = 1;
        cls_shape[1]=bbox_shape[1]=ce_shape[1] = (shape[1]);
        cls_shape[2]=bbox_shape[2]=ce_shape[2] = (shape[2]);
        cls_shape[3] = (shape[3]); //chn for channel
        bbox_shape[3] = (output_tensors[bbox_idx].properties.alignedShape.dimensionSize[3]);
        ce_shape[3] = (output_tensors[ce_idx].properties.alignedShape.dimensionSize[3]);

        
        auto cls_mem =&(output_tensors[i].sysMem[0]);
        auto bbox_mem =&(output_tensors[bbox_idx].sysMem[0]);
        auto ce_mem =&(output_tensors[ce_idx].sysMem[0]);
        // flush to cpu cache
        hbSysFlushMem(cls_mem, HB_SYS_MEM_CACHE_INVALIDATE);
        hbSysFlushMem(bbox_mem, HB_SYS_MEM_CACHE_INVALIDATE);
        hbSysFlushMem(ce_mem, HB_SYS_MEM_CACHE_INVALIDATE);

        // 使用 xt::adapt 将数据从指针视图到 xtensor 数组
        xt::xarray<float> cls = xt::xarray<float>::from_shape(cls_shape);
        xt::xarray<float> bbox_mat = xt::xarray<float>::from_shape(bbox_shape);
        xt::xarray<float> ce = xt::xarray<float>::from_shape(ce_shape);

        
        // get output data
        memcpy(cls.data(), (unsigned char *)output_tensors[i].sysMem[0].virAddr, cls.size() * CV_32FC1_SIZE);
        memcpy(bbox_mat.data(), (unsigned char *)output_tensors[bbox_idx].sysMem[0].virAddr, bbox_mat.size() * CV_32FC1_SIZE);
        memcpy(ce.data(), (unsigned char *)output_tensors[ce_idx].sysMem[0].virAddr, ce.size() * CV_32FC1_SIZE);

        // release hbMem
        hbSysFreeMem(&(output_tensors[i].sysMem[0]));
        hbSysFreeMem(&(output_tensors[bbox_idx].sysMem[0]));
        hbSysFreeMem(&(output_tensors[ce_idx].sysMem[0]));

        cls = 1 / (1 + xt::exp(-cls));
        ce = 1 / (1 + xt::exp(-ce));
        auto score =  xt::sqrt(cls * ce);

        auto classes = xt::argmax(score, 3);
        classes.reshape({-1,1});
        auto _max_score = xt::amax(score, 3);
        xt::xarray<float> max_score = xt::reshape_view(_max_score, {-1, 1});

        xt::xarray<float>adjust_bbox = adjust_bbox(bbox_mat, strides[i], origin_shape, input_size);
        adjust_bbox.reshape({-1, 4});

        auto pred_bboxes = xt::concatenate(xt::xtuple(adjust_bbox, max_score, classes), 1);
        auto index = xt::view(pred_bboxes, xt::all(), 4) > score_threshold;

        // 筛选后的结果
        xt::xarray<float> pred_bbox_filtered = xt::filter(pred_bboxes, index);

        
        results.push_back(pred_bbox_filtered);
    }

    return results;
}

inline xt::xarray<float> bboxes_iou(const xt::xarray<float>& boxes1, const xt::xarray<float>& boxes2) {
    auto boxes1_area = (boxes1(2) - boxes1(0)) * (boxes1(3) - boxes1(1));
    auto boxes2_area = (boxes2(2) - boxes2(0)) * (boxes2(3) - boxes2(1));
    
    xt::xarray<float> left_up = xt::maximum(xt::view(boxes1, xt::range(0, 2)), xt::view(boxes2, xt::range(0, 2)));
    xt::xarray<float> right_down = xt::minimum(xt::view(boxes1, xt::range(2, boxes1.shape()[0])), xt::view(boxes2, xt::range(2, boxes2.shape()[0])));

    xt::xarray<float> inter_section = xt::maximum(right_down - left_up, 0.0);
    xt::xarray<float> inter_area = inter_section(0) * inter_section(1);
    xt::xarray<float> union_area = boxes1_area + boxes2_area - inter_area;
    xt::xarray<float> ious = xt::maximum(inter_area / union_area, std::numeric_limits<float>::epsilon());

    return ious;
}

inline std::vector<xt::xarray<float>> nms(const std::vector<xt::xarray<float>>& bboxes, float iou_threshold) {
    std::set<int> classes_in_img;
    for (const auto& bbox : bboxes) {
        classes_in_img.insert(static_cast<int>(bbox(5)));
    }
    std::vector<xt::xarray<float>> best_bboxes;

    for (int cls : classes_in_img) {
        std::vector<xt::xarray<float>> cls_bboxes;
        for (const auto& bbox : bboxes) {
            if (static_cast<int>(bbox(5)) == cls) {
                cls_bboxes.push_back(bbox);
            }
        }

        while (!cls_bboxes.empty()) {
            auto max_it = std::max_element(cls_bboxes.begin(), cls_bboxes.end(),
                                           [](const xt::xarray<float>& a, const xt::xarray<float>& b) { return a(4) < b(4); });
            xt::xarray<float> best_bbox = *max_it;
            best_bboxes.push_back(best_bbox);
            cls_bboxes.erase(max_it);

            std::vector<xt::xarray<float>> remaining_bboxes;
            std::copy_if(cls_bboxes.begin(), cls_bboxes.end(), std::back_inserter(remaining_bboxes),
                         [&best_bbox, &iou_threshold](const xt::xarray<float>& bbox) {
                            xt::xarray<float> iou = bboxes_iou(xt::view(best_bbox, xt::all(), xt::range(0, 4)), xt::view(bbox, xt::all(), xt::range(0, 4)));
                             return iou(0) <= iou_threshold;
                         });
            cls_bboxes = std::move(remaining_bboxes);
        }
    }
    return best_bboxes;
}

std::vector<xt::xarray<float>> FCOSDetector::postprocess(hbDNNTensor* model_output,
                                           const cv::Size& model_hw_shape,
                                           const cv::Size& origin_img_shape,
                                           float score_threshold,
                                           float nms_threshold) {
    int input_height = model_hw_shape.height;
    int input_width = model_hw_shape.width;

    auto prediction_bbox = decode(model_output, score_threshold, origin_img_shape, 512);
    auto nms_bboxes = nms(prediction_bbox, nms_threshold);

    // Convert to xtensor array
    std::vector<std::size_t> shape = {nms_bboxes.size(), nms_bboxes[0].size()};
    xt::xarray<float> prediction_bbox_array = xt::adapt(nms_bboxes[0].data(), nms_bboxes[0].size(), xt::no_ownership(), shape);

    // Limit the number of predictions to the top 10
    std::size_t topk = std::min(prediction_bbox_array.shape()[0], static_cast<std::size_t>(10));
    std::vector<xt::xarray<float>> topk_bboxes(topk);

    for (std::size_t i = 0; i < topk; ++i) {
        topk_bboxes[i] = nms_bboxes[i];
    }

    return topk_bboxes;
}