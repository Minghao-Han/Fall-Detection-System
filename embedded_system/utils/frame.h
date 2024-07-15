#ifndef _FRAME_H
#define _FRAME_H 1

// #define FRAME_WIDTH 640
// #define FRAME_HEIGHT 480
#include <stddef.h>

typedef unsigned char frame_t;
typedef struct frame_buf_t {
    size_t frame_size;
    frame_t *frames;
    size_t length;
    size_t current_ptr;
} frame_buf_t;

// for yuv420p
frame_buf_t *frame_buf_init4yuv(size_t frame_size, size_t length);
// for normal rgb/bgr
frame_buf_t *frame_buf_init(size_t width, size_t height,int channel_num, size_t length);
void frame_buf_destroy(frame_buf_t *frame_buf);

void frame_buf_push(frame_buf_t *frame_buf, frame_t *frame);

#endif // _FRAME_H