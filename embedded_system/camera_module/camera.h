#ifndef _CAMERA_H
#define _CAMERA_H 1
#include "frame.h"

typedef struct camera_t{
    frame_t *frame_buf;
    void *vio_object;
}camera_t;

camera_t *camera_init();
void camera_start(camera_t *camera);
void camera_destroy(camera_t *camera);

#endif // _CAMERA_H