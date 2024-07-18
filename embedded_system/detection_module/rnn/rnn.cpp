#include "rnn.hpp"
#include <string>
#include <stddef.h>
#include <stdlib.h>

RNNDetector::RNNDetector(const char* model_path){
    dnn_handle = NULL;
    input = NULL;
    output_property = NULL;
    model_output_count = 0;
    // 第一步加载模型
    // const char* model_file_name= "./mobilenetv1.bin";
    hbDNNInitializeFromFiles(&(this->packed_dnn_handle), &model_path, 1);

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
    for (int i = 0; i < model_output_count; i++) {
            hbDNNGetOutputTensorProperties(&(this->output_property) ,dnn_handle, i);
        }
}


RNNDetector::~RNNDetector(){
    if (packed_dnn_handle != NULL) {
        hbDNNRelease(packed_dnn_handle);
    }
};

void RNNDetector::fall_detect(bool *continue_detect,sem_t *fall_sem,int idx,float threshold){
    while(true){
        if(!*continue_detect) break;

        // get kpts and bbox
        DetectBox detect_result = inputss[idx].front();
        // inference
        // 推理
        // 第四步准备输入数据
        // input:float32[batch,3,256,192]
        hbDNNTensor input_tensor;
        input_tensor.properties = input_properties;
        auto &mem = input_tensor.sysMem[0];
        int data_length = 17*3*5*4; //要改
        hbSysAllocCachedMem(&mem, data_length);
        input_tensor.properties.alignedShape = input_tensor.properties.validShape;
        memcpy(mem.virAddr, &detect_result, data_length);
        hbSysFlushMem(&mem, HB_SYS_MEM_CACHE_CLEAN);


        // 准备输出 应该只有一个
        hbDNNTensor *output_tensors = new hbDNNTensor[1];
        output_tensors[0].properties = output_property;
        hbSysMem &mem = output_tensors[0].sysMem[0];
        hbSysAllocCachedMem(&mem, output_property.alignedByteSize);

        // 第六步推理模型
        hbDNNTaskHandle_t task_handle = nullptr;
        hbDNNInferCtrlParam infer_ctrl_param;
        HB_DNN_INITIALIZE_INFER_CTRL_PARAM(&infer_ctrl_param);
        hbDNNInfer(&task_handle,&output_tensors,&input_tensor,dnn_handle,&infer_ctrl_param);

        // 第七步等待任务结束
        hbDNNWaitTaskDone(task_handle, 0);
        hbDNNReleaseTask(task_handle);

        // 只有一个output tensor
        auto &output_mem = output_tensors[0].sysMem[0];
        hbSysFlushMem(&output_mem, HB_SYS_MEM_CACHE_INVALIDATE);
        float *output_data = reinterpret_cast<float *>(output_mem.virAddr);
        if (output_data[0] > threshold){
            sem_post(fall_sem);
        }
        hbSysFreeMem(&output_mem);

    }
};