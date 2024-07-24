#ifndef _CAMERA_H
#define _CAMERA_H 1
#include "frame.h"
#include <stdint.h>
#include <sp_vio.h>

typedef struct camera_t{
    frame_buf_t *camera_buf;
    size_t frame_size;
    void *vio_camera_ptr;
    int32_t yuv_width;
    int32_t yuv_height;
}camera_t;

camera_t *camera_init(sp_sensors_parameters *camera_params,int channel_num,frame_buf_t *camera_buf);

/** 
 * @brief Start the camera module
 * @param camera: camera_t *
*/
void *camera_start(void *args);
void camera_destroy(camera_t *camera);

#endif // _CAMERA_H