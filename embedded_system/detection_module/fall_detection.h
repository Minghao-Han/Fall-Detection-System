#ifndef _FALL_DETECTION_H
#define _FALL_DETECTION_H 1
#include <semaphore.h>
#include "frame.h"
#include "bytetrack-cpp/include/BYTETracker.h"
#include "lstm.hpp"
#include "rtm.cpp"

typedef struct fall_detector_t {
    sem_t *fall_sem; //if fall than post a sem
    frame_buf_t *frame_buf; // store processed fall clip
    Detector *bbox_kpts_detector;
    BYTETracker *byte_tracker;
    LSTMDetector *lstm_analyzer;
} fall_detector_t;

fall_detector_t *fall_detector_init(sem_t *fall_sem,frame_buf_t *frame_buf);

/**
 * @brief Start the fall detection module
 * @param fall_detector: fall_detector_t *
 */
void *fall_detector_start(void *args);
void fall_detector_destroy(fall_detector_t *fall_detector);

#endif