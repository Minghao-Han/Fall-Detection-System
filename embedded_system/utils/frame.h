#ifndef _FRAME_H
#define _FRAME_H 1

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#include <stddef.h>

typedef unsigned char frame_t;

frame_t *frame_buf_init(size_t width, size_t height,int channel_num);

#endif // _FRAME_H