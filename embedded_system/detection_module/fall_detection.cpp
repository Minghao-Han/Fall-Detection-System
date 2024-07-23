#include "fall_detection.h"
#include "thread_pool.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <thread>
#include <unistd.h>
#include <dnn/hb_dnn.h>
#include <dnn/hb_sys.h>
#include "sp_codec.h"
#include "sp_vio.h"
#include "sp_sys.h"
#include "sp_display.h"

static const char *MBFALL_MODEL_PATH = "/home/sunrise/models/mobilenetv3-fall.bin";
static const int8_t THREAD_NUM_MAX = 5;
static const size_t SIZE_OF_FLOAT = sizeof(float);
static const float FALL_THRESHOLD = 0.85;
static bool detect_continue = false;
static void *vps;
static int input_size;
static int output_size;

fall_detector_t *fall_detector_init(sem_t *fall_sem, frame_buf_t *clip) {
    fall_detector_t *fall_detector = (fall_detector_t *)malloc(sizeof(fall_detector_t));
    if (fall_detector == NULL) {
        return NULL;
    }
    fall_detector->fall_sem = fall_sem;
    fall_detector->frame_buf = clip;
    // init vps
    vps = sp_init_vio_module();
    // init heap memory
    input_size = FRAME_BUFFER_SIZE(640, 480);
    output_size = FRAME_BUFFER_SIZE(224, 224);
    // set vps to scale only mode
    int input_width=640;
    int input_height=480;
    int output_width=224;
    int out_height=224;
    // set vps to scale only mode
    int ret = sp_open_vps(vps, 1, 1, SP_VPS_SCALE, input_width, input_height, &output_width, &out_height, NULL, NULL, NULL, NULL, NULL);
    if (ret != 0)
    {
        printf("[Error] sp_open_vps failed!\n");
        goto exit;
    }
    detect_continue = true;
    return fall_detector;
exit:
    // Module stop
    sp_vio_close(vps);
    // Object release
    sp_release_vio_module(vps);
    return 0;
}

void *fall_detector_start(void *args) {
    fall_detector_t *fall_detector = (fall_detector_t *)args;
    sem_t *fall_sem = fall_detector->fall_sem;
    frame_buf_t *frame_buf = fall_detector->frame_buf;

        // 第一步加载模型
    hbPackedDNNHandle_t packed_dnn_handle;
    hbDNNInitializeFromFiles(&packed_dnn_handle, &MBFALL_MODEL_PATH, 1);

    // 第二步获取模型名称
    const char **model_name_list;
    int model_count = 0;
    hbDNNGetModelNameList(&model_name_list, &model_count, packed_dnn_handle);

    // 第三步获取dnn_handle
    hbDNNHandle_t dnn_handle;
    hbDNNGetModelHandle(&dnn_handle, packed_dnn_handle, model_name_list[0]);

    // 第四步准备输入数据
    hbDNNTensor input;
    hbDNNTensorProperties input_properties;
    hbDNNGetInputTensorProperties(&input_properties, dnn_handle, 0);
    input.properties = input_properties;
    auto &in_mem = input.sysMem[0];

    int yuv_length = FRAME_BUFFER_SIZE(640, 480);
    hbSysAllocCachedMem(&in_mem, FRAME_BUFFER_SIZE(224, 224));

    // 第五步准备模型输出数据的空间
    int model_output_count;
    if (hbDNNGetOutputCount(&model_output_count, dnn_handle) != 0) {
        printf("Failed to get output count\n");
        hbSysFreeMem(&in_mem); // 释放资源
        hbDNNRelease(packed_dnn_handle); // 释放资源
        return NULL;
    }
    // model_output_count应该为1
    hbDNNTensor *output_tensors = new hbDNNTensor[model_output_count];
    for (int i = 0; i < model_output_count; i++) {
        hbDNNTensorProperties &output_property = output_tensors[i].properties;
        if (hbDNNGetOutputTensorProperties(&output_property, dnn_handle, i) != 0) {
            printf("Failed to get output tensor properties for tensor %d\n", i);
            delete[] output_tensors; // 释放资源
            hbSysFreeMem(&in_mem); // 释放资源
            hbDNNRelease(packed_dnn_handle); // 释放资源
            return NULL;
        }
        hbSysMem &in_mem = output_tensors[i].sysMem[0];
        // printf("Allocating memory for output tensor %d, size: %d\n", i, output_property.alignedByteSize);
        if (hbSysAllocCachedMem(&in_mem, output_property.alignedByteSize) != 0) {
            printf("Failed to allocate memory for output tensor %d\n", i);
            delete[] output_tensors; // 释放资源
            hbSysFreeMem(&in_mem); // 释放资源
            hbDNNRelease(packed_dnn_handle); // 释放资源
            return NULL;
        }
    }

    printf("fall detector start\n");
    hbSysMem *probability_mem = &(output_tensors[0].sysMem[0]);
    void *probability_mem_virAddr = probability_mem->virAddr;
    float probabilities[2] = {0.0, 0.0}; // 初始化概率数组大小
    // frame_t *frame = (frame_t *)malloc(yuv_length);
    frame_t *frame = (frame_t *)malloc(yuv_length);



    if (frame == NULL) {
        printf("Failed to allocate memory for frame\n");
        for (int i = 0; i < model_output_count; i++) {
            hbSysFreeMem(&(output_tensors[i].sysMem[0]));
        }
        delete[] output_tensors;
        hbSysFreeMem(&in_mem);
        hbDNNRelease(packed_dnn_handle);
        return NULL;
    }
    int8_t fall_count = 0;
    int8_t skip_frame = 2;
    do {
        // detection too fast, skip some frames
        if (skip_frame)
        {   
            skip_frame>>1;
            continue;
        }
        skip_frame=2;
        // 从 frame_buf 中取出一帧
        frame_buf_pop(frame_buf, frame);
        int ret = sp_vio_set_frame(vps, frame, input_size);
        if (ret != 0){
            printf("[Error] sp_vio_set_frame from vps failed!\n");
            break;
        }
        // Get the processed image
        ret = sp_vio_get_frame(vps, (char *)in_mem.virAddr, 224, 224, 2000);
        if (ret != 0)
        {
            printf("[Error] sp_vio_get_frame from vps failed!\n");
            break;
        }
        // memcpy(in_mem.virAddr, frame, yuv_length);
        // 将 frame 数据写入 input_tensors[0]
        hbSysFlushMem(&in_mem, HB_SYS_MEM_CACHE_CLEAN);

        // 第六步推理模型
        hbDNNInferCtrlParam infer_ctrl_param;
        HB_DNN_INITIALIZE_INFER_CTRL_PARAM(&infer_ctrl_param);
        hbDNNTaskHandle_t task_handle = nullptr;
        if (hbDNNInfer(&task_handle, &output_tensors, &input, dnn_handle, &infer_ctrl_param) != 0) {
            printf("Failed to infer\n");
        }

        // 第七步等待任务结束
        if (hbDNNWaitTaskDone(task_handle, 0) != 0) {
            printf("Failed to wait for task done\n");
            hbDNNReleaseTask(task_handle);
            free(frame); // 释放frame内存
            for (int i = 0; i < model_output_count; i++) {
                hbSysFreeMem(&(output_tensors[i].sysMem[0]));
            }
            delete[] output_tensors;
            hbSysFreeMem(&in_mem);
            hbDNNRelease(packed_dnn_handle);
            return NULL;
        }
        hbDNNReleaseTask(task_handle);

        // 概率输出是第一个输出
        hbSysFlushMem(probability_mem, HB_SYS_MEM_CACHE_INVALIDATE);
        memcpy(probabilities, probability_mem_virAddr, SIZE_OF_FLOAT * 2); // 复制两个浮点数
        float no_fall = probabilities[0];
        float fall = probabilities[1];
        // 使用输出数据
        if (fall - no_fall > FALL_THRESHOLD) {
            fall_count++;
        }
        if (fall_count > 3) {
            sem_post(fall_sem);
            fall_count = 0;
        }
    } while (detect_continue);

    // 第八步释放资源
    free(frame); // 释放frame内存
    hbSysFreeMem(&in_mem);
    for (int i = 0; i < model_output_count; i++) {
        hbSysFreeMem(&(output_tensors[i].sysMem[0]));
    }
    delete[] output_tensors;
    hbDNNRelease(packed_dnn_handle);
    printf("fall detector stopped\n");

    return NULL;
}

void fall_detector_destroy(fall_detector_t *fall_detector) {
    detect_continue = false;
    // Module stop
    sp_vio_close(vps);
    // Object release
    sp_release_vio_module(vps);
    free(fall_detector);
}
