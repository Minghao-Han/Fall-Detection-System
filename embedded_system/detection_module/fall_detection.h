#ifndef _FALL_DETECTION_H
#define _FALL_DETECTION_H 1
#include <semaphore.h>
#include "frame.h"
#include "inference.h"
#include "thread_pool.h"

typedef struct fall_detector_t {
    frame_t *frame_buf;
    sem_t *fall_sem;
    thread_pool_t *thread_pool;
    hbDNNHandle_t dnn_handle;
    hbDNNTaskHandle_t *dnn_task_handles;
} fall_detector_t;

fall_detector_t *fall_detector_init(frame_t *frame_buf, sem_t *fall_sem, int thread_num);
void fall_detector_start(fall_detector_t *fall_detector);
void fall_detector_destroy(fall_detector_t *fall_detector);

#endif