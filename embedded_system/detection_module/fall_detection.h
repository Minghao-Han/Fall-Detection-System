#ifndef _FALL_DETECTION_H
#define _FALL_DETECTION_H 1
#include <semaphore.h>
#include "frame.h"

typedef struct fall_detector_t {
    frame_t *camera_buf;
    sem_t *fall_sem; //if fall than post a sem
    frame_buf_t *clip; // store processed fall clip
} fall_detector_t;

fall_detector_t *fall_detector_init(frame_t *camera_buf, sem_t *fall_sem,frame_buf_t *clip);

/**
 * @brief Start the fall detection module
 * @param fall_detector: fall_detector_t *
 */
void *fall_detector_start(void *);
void fall_detector_destroy(fall_detector_t *fall_detector);

#endif