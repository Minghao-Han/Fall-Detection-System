#include "fall_detection.h"
#include "thread_pool.h"
#include <stdlib.h>
#include <time.h> 
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include "rtm.cpp"
#include "BYTETracker.h"
#include "rnn.hpp"

#define FCOS_MODEL_PATH "mymodel/fall_detection_model.bin"
#define RTMPOSE_MODEL_PATH "mymodel/rtmpose_model.bin"
#define RNN_MODEL_PATH "mymodel/rnn_model.bin"

static int8_t THREAD_NUM_MAX = 5; 

static bool detect_continue=false;
static std::queue<Result> bbox_kpts_results;

fall_detector_t *fall_detector_init(sem_t *fall_sem,frame_buf_t *clip){
    fall_detector_t *fall_detector = (fall_detector_t *)malloc(sizeof(fall_detector_t));
    if(fall_detector == NULL){
        return NULL;
    }
    fall_detector->fall_sem = fall_sem;
    fall_detector->frame_buf = clip;
    fall_detector->bbox_kpts_detector = new Detector(bbox_kpts_results,FCOS_MODEL_PATH,RTMPOSE_MODEL_PATH);
    fall_detector->byte_tracker = new BYTETracker(10,30);
    fall_detector->rnn_analyzer = new RNNDetector(RNN_MODEL_PATH);
    detect_continue=true;
    return fall_detector;
}

void get_bbox_kpts_task(Detector* obj, bool *detect_continue) {
    obj->detect_bbox_kpts(detect_continue);
}

void *fall_detector_start(void *args){
    fall_detector_t *fall_detector=(fall_detector_t *)args;
    frame_t *a_frame=(frame_t *)malloc(fall_detector->frame_buf->frame_size);
    printf("fall detector start\n");
    // 创建一个线程向量
    std::vector<std::thread> bbox_kpts_threads;

    // 启动多个线程来调用类的成员函数
    for (int i = 0; i < THREAD_NUM_MAX; ++i) {
        bbox_kpts_threads.emplace_back(threadFunction, fall_detector->bbox_kpts_detector, &detect_continue);
    }
    BYTETracker *byte_tracker = (BYTETracker *)fall_detector->byte_tracker;
    RNNDetector *rnn_analyzer = (RNNDetector *)fall_detector->rnn_analyzer;
    vector<DetectBox> bbox;
    vector<int> ids;
    do{
        bbox = bbox_kpts_results.front().bboxes;
        if (bbox.size() == 0)continue;
        ids=byte_tracker->update(bbox);
        for(auot id:ids){
            rnn_analyzer->intput.push(bbox[id]);
        }
        bbox_kpts_results.pop();
    }while(1);
    // 等待所有线程完成
    for (auto& bbox_kpts_thread : bbox_kpts_threads) {
        bbox_kpts_thread.join();
    }
}
void fall_detector_destroy(fall_detector_t *fall_detector){
    detect_continue=false;
    free(fall_detector);
}