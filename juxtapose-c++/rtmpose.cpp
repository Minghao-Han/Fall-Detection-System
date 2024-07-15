#include "rtmpose.hpp"
#include <iostream>
#define SIZE_OF_INT 4
#define SIZE_OF_FLOAT 4


const int32_t RTMPose::SIMCC_LEN[2] = {SIMCC_X_LEN, SIMCC_Y_LEN};

RTMPose::RTMPose(const char* model_path, const string& device) {
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
    hbDNNGetOutputCount(&model_output_count, dnn_handle);

    

    // const char* providers[] = {device == "cuda" ? "CUDAExecutionProvider" : "CPUExecutionProvider"};

    model_input_size = Size(192, 256);
    mean = Scalar(123.675, 116.28, 103.53);
    std = Scalar(58.395, 57.12, 57.375);
    // this->device = device;

    std::cout << "Loaded " << model_path << " onnx model in."  << endl;
}

std::vector<std::vector<PosePoint>> RTMPose::operator()(const cv::Mat& input_mat, const vector<DetectBox>& boxs)
{
    std::vector<std::vector<PosePoint>> kpts;
	for (auto& box : boxs){
        std::vector<PosePoint> pose_result;

        if (!box.IsValid())
            kpts.push_back(pose_result);

        std::pair<cv::Mat, cv::Mat> crop_result_pair = CropImageByDetectBox(input_mat, box);

        cv::Mat crop_mat = crop_result_pair.first;
        cv::Mat affine_transform_reverse = crop_result_pair.second;

        // deep copy
        // 深拷贝
        cv::Mat crop_mat_copy;
        crop_mat.copyTo(crop_mat_copy);

        // BGR to RGB
        // BGR转RGB
        cv::Mat input_mat_copy_rgb;
        cv::cvtColor(crop_mat, input_mat_copy_rgb, CV_BGR2RGB);

        // image data，HWC->CHW，image_data - mean / std normalize
        // 图片数据，HWC->CHW，image_data - mean / std归一化
        int image_height = input_mat_copy_rgb.rows;
        int image_width = input_mat_copy_rgb.cols;
        int image_channels = input_mat_copy_rgb.channels();

        std::vector<float> input_image_array;
        input_image_array.resize(1 * image_channels * image_height * image_width);

        float* input_image = input_image_array.data();
        for (int h = 0; h < image_height; ++h)
        {
            for (int w = 0; w < image_width; ++w)
            {
                for (int c = 0; c < image_channels; ++c)
                {
                    int chw_index = c * image_height * image_width + h * image_width + w;

                    float tmp = input_mat_copy_rgb.ptr<uchar>(h)[w * 3 + c];

                    input_image[chw_index] = (tmp - IMAGE_MEAN[c]) / IMAGE_STD[c];
                }
            }
        }




        // inference
        // 推理
        // 第四步准备输入数据
        // input:float32[batch,3,256,192]
        hbDNNTensor input_tensor;
        input_tensor.properties = input_properties;
        auto &mem = input_tensor.sysMem[0];
        int data_length = 1*3*256*192;
        hbSysAllocCachedMem(&mem, data_length);
        input_tensor.properties.alignedShape = input_tensor.properties.validShape;
        memcpy(mem.virAddr, input_image, input_image_array.size());
        hbSysFlushMem(&mem, HB_SYS_MEM_CACHE_CLEAN);


        // 准备输出
        hbDNNTensor *output_tensors = new hbDNNTensor[model_output_count];
        for (int i = 0; i < model_output_count; i++) {
            hbDNNTensorProperties &output_properties = output_tensors[i].properties;
            hbDNNGetOutputTensorProperties(&output_properties, dnn_handle, i);

            // // 获取模型输出尺寸
            // int out_aligned_size = 4;
            // for (int j = 0; j < output_properties.alignedShape.numDimensions; j++) {
            //     out_aligned_size =
            //         out_aligned_size * output_properties.alignedShape.dimensionSize[j];
            // }

            hbSysMem &mem = output_tensors[i].sysMem[0];
            hbSysAllocCachedMem(&mem, output_properties.alignedByteSize);
        }

        // 第六步推理模型
        hbDNNTaskHandle_t task_handle = nullptr;
        hbDNNInferCtrlParam infer_ctrl_param;
        HB_DNN_INITIALIZE_INFER_CTRL_PARAM(&infer_ctrl_param);
        hbDNNInfer(&task_handle,&output_tensors,&input_tensor,dnn_handle,&infer_ctrl_param);

        // 第七步等待任务结束
        hbDNNWaitTaskDone(task_handle, 0);
        hbDNNReleaseTask(task_handle);

        // 第八步获取输出数据
        float simcc_x_result[SIMCC_X_LEN];
        float simcc_y_result[SIMCC_Y_LEN];
        float *simmc_result[2]={simcc_x_result,simcc_y_result};

        int simcc_x_dims[3];
        int simcc_y_dims[3];
        int *simcc_dims[2]={simcc_x_dims,simcc_y_dims};
        
        for(int i=0;i<model_output_count;i++){
            hbDNNTensor *tensor = output_tensors+i;
            hbSysMem *mem = &(tensor->sysMem[0]);
            // output_tensors bpu->cpu
            hbSysFlushMem(mem, HB_SYS_MEM_CACHE_INVALIDATE);
            int *simcc_shape = tensor->properties.validShape.dimensionSize;
            memcpy(simcc_dims[i], simcc_shape, 3 * SIZE_OF_INT);
            // The type reinterpret_cast should be determined according to the output type
            // For example: HB_DNN_TENSOR_TYPE_F32 is float
            memcpy((void *)(simmc_result[i]), (void *)(mem->virAddr), SIMCC_LEN[i] * SIZE_OF_FLOAT);
            // 释放ouput内存
            hbSysFreeMem(&(tensor->sysMem[0]));
        }
    
        // 释放input内存
        hbSysFreeMem(&(input_tensor.sysMem[0]));

        // Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
        //     memory_info,
        //     input_image_array.data(),
        //     input_image_array.size(),
        //     input_shape.data(),
        //     input_shape.size()
        // );

        // pose process
        // 后处理
        // assert(simcc_x_dims.size() == 3 && simcc_y_dims.size() == 3);

        int batch_size = simcc_x_dims[0] == simcc_y_dims[0] ? simcc_x_dims[0] : 0;
        int joint_num = simcc_x_dims[1] == simcc_y_dims[1] ? simcc_x_dims[1] : 0;
        int extend_width = simcc_x_dims[2];
        int extend_height = simcc_y_dims[2];

        
        for (int i = 0; i < joint_num; ++i)
        {
            // find the maximum and maximum indexes in the value of each Extend_width length
            // 在每一个extend_width长度的数值中找到最大值以及最大值的索引
            auto x_biggest_iter = std::max_element(simcc_x_result + i * extend_width, simcc_x_result + i * extend_width + extend_width);
            int max_x_pos = std::distance(simcc_x_result + i * extend_width, x_biggest_iter);
            int pose_x = max_x_pos / 2;
            float score_x = *x_biggest_iter;

            // find the maximum and maximum indexes in the value of each exten_height length
            // 在每一个extend_height长度的数值中找到最大值以及最大值的索引
            auto y_biggest_iter = std::max_element(simcc_y_result + i * extend_height, simcc_y_result + i * extend_height + extend_height);
            int max_y_pos = std::distance(simcc_y_result + i * extend_height, y_biggest_iter);
            int pose_y = max_y_pos / 2;
            float score_y = *y_biggest_iter;

            //float score = (score_x + score_y) / 2;
            float score = std::max(score_x, score_y);

            PosePoint temp_point;
            temp_point.x = int(pose_x);
            temp_point.y = int(pose_y);
            temp_point.score = score;
            pose_result.emplace_back(temp_point);
        }

        // anti affine transformation to obtain the coordinates on the original picture
        // 反仿射变换获取在原始图片上的坐标
        for (int i = 0; i < pose_result.size(); ++i)
        {
            cv::Mat origin_point_Mat = cv::Mat::ones(3, 1, CV_64FC1);
            origin_point_Mat.at<double>(0, 0) = pose_result[i].x;
            origin_point_Mat.at<double>(1, 0) = pose_result[i].y;

            cv::Mat temp_result_mat = affine_transform_reverse * origin_point_Mat;

            pose_result[i].x = static_cast<float>(temp_result_mat.at<double>(0, 0));
            pose_result[i].y = static_cast<float>(temp_result_mat.at<double>(1, 0));
        }

        kpts.push_back(pose_result);
    }
    return kpts;
}

std::pair<cv::Mat, cv::Mat> RTMPose::CropImageByDetectBox(const cv::Mat& input_image, const DetectBox& box)
{
	std::pair<cv::Mat, cv::Mat> result_pair;

	if (!input_image.data)
	{
		return result_pair;
	}

	if (!box.IsValid())
	{
		return result_pair;
	}

	// deep copy
	// 深拷贝
	cv::Mat input_mat_copy;
	input_image.copyTo(input_mat_copy);

	// calculate the width, height and center points of the human detection box
	// 计算人体检测框的宽、高以及中心点
	int box_width = box.right - box.left;
	int box_height = box.bottom - box.top;
	int box_center_x = box.left + box_width / 2;
	int box_center_y = box.top + box_height / 2;

	float aspect_ratio = 192.0 / 256.0;

	// adjust the width and height ratio of the size of the picture in the RTMPOSE input
	// 根据rtmpose输入图片大小的宽高比例进行调整
	if (box_width > (aspect_ratio * box_height))
	{
		box_height = box_width / aspect_ratio;
	}
	else if (box_width < (aspect_ratio * box_height))
	{
		box_width = box_height * aspect_ratio;
	}

	float scale_image_width = box_width * 1.25;
	float scale_image_height = box_height * 1.25;

	// get the affine matrix
	// 获取仿射矩阵
	cv::Mat affine_transform = GetAffineTransform(
		box_center_x,
		box_center_y,
		scale_image_width,
		scale_image_height,
		192,
		256
	);
	
	cv::Mat affine_transform_reverse = GetAffineTransform(
		box_center_x,
		box_center_y,
		scale_image_width,
		scale_image_height,
		192,
		256,
		true
	);

	// affine transform
	// 进行仿射变换
	cv::Mat affine_image;
	cv::warpAffine(input_mat_copy, affine_image, affine_transform, cv::Size(192, 256), cv::INTER_LINEAR);
	//cv::imwrite("affine_img.jpg", affine_image);

	result_pair = std::make_pair(affine_image, affine_transform_reverse);

	return result_pair;
}

