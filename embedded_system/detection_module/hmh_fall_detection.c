#include "fall_detection.h"
#include "thread_pool.h"

#define MODEL_PATH "mymodel/fall_detection_model.bin"

fall_detector_t *fall_detector_init(frame_t *frame_buf, sem_t *fall_sem, int thread_num){
    fall_detector_t *fall_detector = (fall_detector_t *)malloc(sizeof(fall_detector_t));
    if(fall_detector == NULL){
        return NULL;
    }
    fall_detector->frame_buf = frame_buf;
    fall_detector->fall_sem = fall_sem;
    int init_result = thread_pool_init(&(fall_detector->thread_pool), thread_num);
    if(init_result){
        free(fall_detector);
        return NULL;
    }

    // model init 模型初始化
    // 第一步加载模型
    hbPackedDNNHandle_t packed_dnn_handle;
    const char* model_file_name= MODEL_PATH;
    hbDNNInitializeFromFiles(&packed_dnn_handle, &model_file_name, 1);

    // 第二步获取模型名称
    const char **model_name_list;
    int model_count = 0;
    hbDNNGetModelNameList(&model_name_list, &model_count, packed_dnn_handle);

    // 第三步获取dnn_handle
    hbDNNGetModelHandle(&(fall_detector->dnn_handle), packed_dnn_handle, model_name_list[0]);

    // 第四步创建task_handle数组
    hbDNNTaskHandle_t *dnn_task_handles = (hbDNNTaskHandle_t *)malloc(sizeof(hbDNNTaskHandle_t) * thread_num);

    inference_init();
    return fall_detector;

}
void fall_detector_start(fall_detector_t *fall_detector);
void fall_detector_destroy(fall_detector_t *fall_detector){
    thread_pool_destroy(&(fall_detector->thread_pool));
    inference_destroy();
    free(fall_detector);
}