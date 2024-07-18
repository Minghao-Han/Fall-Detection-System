#ifndef _FALL_DETECTION_H
#define _FALL_DETECTION_H 1
#include <semaphore.h>
#include "frame.h"

typedef struct fall_detector_t {
    sem_t *fall_sem; //if fall than post a sem
    frame_buf_t *frame_buf; // store processed fall clip
    void *bbox_kpts_detector;
    void *byte_tracker;
    void *rnn_analyzer;
} fall_detector_t;

fall_detector_t *fall_detector_init(sem_t *fall_sem,frame_buf_t *frame_buf);

/**
 * @brief Start the fall detection module
 * @param fall_detector: fall_detector_t *
 */
void *fall_detector_start(void *args);
void fall_detector_destroy(fall_detector_t *fall_detector);

#endif