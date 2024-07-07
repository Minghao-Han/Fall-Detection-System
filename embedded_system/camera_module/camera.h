#ifndef _CAMERA_H
#define _CAMERA_H 1
#include "frame.h"

typedef struct camera_t{
    frame_t *camera_buf;
    void *vio_object;
}camera_t;

camera_t *camera_init();

/** 
 * @brief Start the camera module
 * @param camera: camera_t *
*/
void *camera_start(void *args);
void camera_destroy(camera_t *camera);

#endif // _CAMERA_H