#ifndef _CAMERA_H
#define _CAMERA_H 1
#include "frame.h"

typedef struct camera_t{
    frame_t *camera_buf;
    size_t frame_size;
    void *vio_object;
}camera_t;

camera_t *camera_init(size_t width, size_t height,int channel_num);

/** 
 * @brief Start the camera module
 * @param camera: camera_t *
*/
void *camera_start(void *args);
void camera_destroy(camera_t *camera);

#endif // _CAMERA_H