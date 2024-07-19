#include "./lstm.hpp"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <array>

LSTMDetector::LSTMDetector(const char* model_path){
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
    hbDNNGetInputTensorProperties(&(input_property), dnn_handle, 0);
    hbDNNGetInputTensorProperties(&(hidden_property), dnn_handle, 1);
    hbDNNGetInputTensorProperties(&(c_property), dnn_handle, 2);
    // 第五步准备模型输出数据的空间
    hbDNNGetOutputCount(&model_output_count, dnn_handle);
    for (int i = 0; i < model_output_count; i++) {
            hbDNNGetOutputTensorProperties(&(this->output_property) ,dnn_handle, i);
        }
}


LSTMDetector::~LSTMDetector(){
    if (packed_dnn_handle != NULL) {
        hbDNNRelease(packed_dnn_handle);
    }
};

void LSTMDetector::fall_detect(bool *continue_detect,sem_t *fall_sem,int idx,float threshold){

    // 准备输入数据
    hbDNNTensor *input_tensors = new hbDNNTensor[3];

    hbDNNTensor &input_tensor=input_tensors[0];
    input_tensor.properties = input_property;
    auto &input_mem = input_tensor.sysMem[0];
    int32_t data_len = 1*56*4; 
    hbSysAllocCachedMem(&input_mem, data_len);
    input_tensor.properties.alignedShape = input_tensor.properties.validShape;

    //hiden tensor: shape{51}
    hbDNNTensor hidden_tensor;
    hidden_tensor.properties = hidden_property;
    auto &hidden_mem = hidden_tensor.sysMem[0];
    hbSysAllocCachedMem(&hidden_mem, 51*4);
    hbSysFlushMem(&hidden_mem, HB_SYS_MEM_CACHE_CLEAN);

    //c tensor: shape{51}
    hbDNNTensor c_tensor;
    c_tensor.properties = c_property;
    auto &c_mem = c_tensor.sysMem[0];
    hbSysAllocCachedMem(&c_mem, 51*4);
    hbSysFlushMem(&c_mem, HB_SYS_MEM_CACHE_CLEAN);

    input_tensors[1] = hidden_tensor;
    input_tensors[2] = c_tensor;

    // 准备输出数据
    hbDNNTensor *output_tensors = new hbDNNTensor[model_output_count];
    // 准备输出 output hidden c
    hbDNNTensor &output_tensor = output_tensors[0];
    output_tensor.properties = output_property;
    hbSysMem &output_mem = output_tensor.sysMem[0];
    hbSysAllocCachedMem(&output_mem, output_property.alignedByteSize);

    output_tensors[1] = hidden_tensor;
    output_tensors[2] = c_tensor;


    hbDNNInferCtrlParam infer_ctrl_param;


    while(true){
        if(!*continue_detect) break;

        // get kpts and bbox
        Result detect_result = inputss[idx].front();
        inputss[idx].pop();
        // inference
        // 推理
        // 第四步准备输入数据
        // input:float32[1,56]
        memcpy(input_mem.virAddr,&detect_result, data_len);
        hbSysFlushMem(&input_mem, HB_SYS_MEM_CACHE_CLEAN);
        // output_tensors[1].properties = hidden_property;
        // hbSysMem &new_hidden_mem = output_tensors[1].sysMem[0];
        // hbSysAllocCachedMem(&new_hidden_mem, hidden_property.alignedByteSize);

        // output_tensors[2].properties = c_property;
        // hbSysMem &new_c_mem = output_tensors[2].sysMem[0];
        // hbSysAllocCachedMem(&new_c_mem, c_property.alignedByteSize);

        hbDNNTaskHandle_t task_handle = nullptr;

        // 第六步推理模型
        HB_DNN_INITIALIZE_INFER_CTRL_PARAM(&infer_ctrl_param);
        hbDNNInfer(&task_handle,&output_tensors,&input_tensor,dnn_handle,&infer_ctrl_param);

        // 第七步等待任务结束
        hbDNNWaitTaskDone(task_handle, 0);
        hbDNNReleaseTask(task_handle);

        // 只有一个output tensor
        // auto &output_mem = output_tensors[0].sysMem[0];
        hbSysFlushMem(&output_mem, HB_SYS_MEM_CACHE_INVALIDATE);
        float *output_data = reinterpret_cast<float *>(output_mem.virAddr);
        if (output_data[0] > threshold){
            sem_post(fall_sem);
        }
        hbSysFreeMem(&output_mem);
        // hbSysFreeMem(&new_hidden_mem);
        // hbSysFreeMem(&new_c_mem);

    }
    hbSysFreeMem(&hidden_mem);
    hbSysFreeMem(&c_mem);
};