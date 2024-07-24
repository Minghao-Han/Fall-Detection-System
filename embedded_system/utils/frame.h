#ifndef _FRAME_H
#define _FRAME_H 1
#ifndef FRAME_BUFFER_SIZE
#define FRAME_BUFFER_SIZE(w, h) ((w) * (h) * (3) / (2))
#endif

// #define FRAME_WIDTH 640
// #define FRAME_HEIGHT 480
#include <stddef.h>
#include <pthread.h>
#include <stdint.h>

// using namespace std;
typedef char frame_t;
typedef struct frame_buf_t {
    size_t frame_size;
    frame_t *frames;
    int16_t length;
    int16_t writer_head;
    pthread_mutex_t *buf_locks; // this lock is for each frame, avoid read and write at the same time
} frame_buf_t;

// for yuv420p
// frame_buf_t *frame_buf_init4yuv(size_t frame_size, size_t length);
// for normal rgb/bgr
frame_buf_t *frame_buf_init(size_t width, size_t height,int channel_num, int16_t length);
void frame_buf_destroy(frame_buf_t *frame_buf);

// frame_buf_push is no need anymore. Because only camera moduel will call it, and now it has already been integrated into camera module.
// void frame_buf_push(frame_buf_t *frame_buf, frame_t *frame);
void frame_buf_pop(frame_buf_t *frame_buf, frame_t *frame);

#endif // _FRAME_H