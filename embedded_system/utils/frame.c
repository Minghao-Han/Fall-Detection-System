#include "frame.h"
#include <stdlib.h>
#include <cstdio>
#include <string.h>


frame_buf_t *frame_buf_init(size_t width, size_t height,int channel_num, int16_t length){
    frame_buf_t *frame_buf = (frame_buf_t *)malloc(sizeof(frame_buf_t));
    if(frame_buf == NULL){
        return NULL;
    }
    //if channel_num==0 means using yuv format
    size_t frame_size;
    if (channel_num==0) frame_size=FRAME_BUFFER_SIZE(width, height);
    else frame_size=width * height * channel_num;
    frame_buf->frame_size=frame_size;
    frame_buf->length = length;
    frame_buf->writer_head = 0;
    frame_buf->frames = (frame_t *)malloc(sizeof(frame_t) * frame_size * length);
    if(frame_buf->frames == NULL){
        free(frame_buf);
        return NULL;
    }
    frame_buf->buf_locks = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * length);
    //initialize the re locks
    for(int i = 0; i < length; i++) pthread_mutex_init(&(frame_buf->buf_locks[i]), NULL);
    return frame_buf;
}
void frame_buf_destroy(frame_buf_t *frame_buf){
    for(int i = 0; i < frame_buf->length; i++) pthread_mutex_destroy(&(frame_buf->buf_locks[i]));
    free(frame_buf->frames);
    free(frame_buf);
}
// frame_buf_push is no need anymore. Because only camera moduel will call it, and now it has already been integrated into camera module.
// void frame_buf_push(frame_buf_t *frame_buf, frame_t *frame){
//     size_t frame_size = frame_buf->frame_size;
//     // char yuv_filename[]="/home/sunrise/clip/buf_push.yuv";
//     // FILE *yuv_file = fopen(yuv_filename, "wb");
//     // fwrite(frame, sizeof(char), 460800, yuv_file);
//     // fflush(yuv_file);
//     // fclose(yuv_file); //?需要吗
//     int16_t writer_head = frame_buf->writer_head;
//     pthread_mutex_lock(frame_buf->buf_locks+writer_head);
//     memcpy(frame_buf->frames + writer_head * frame_size, frame, frame_size);
//     frame_buf->writer_head = (writer_head + 1) % frame_buf->length;
//     pthread_mutex_unlock(frame_buf->buf_locks+writer_head);
// }


void frame_buf_pop(frame_buf_t *frame_buf, frame_t *frame)
{
    // under the assumption that the the speed of camera reading is faster than that of the detection module, these buf_locks avoid duplicate detection of a same frame and meanwhile avoid reading and writing at the same time.
    // deprecated 在假设相机读取速度快于检测模块的情况下，这些buf_locks避免了对同一帧的重复检测，同时避免了读写冲突。
    int16_t read_head = (frame_buf->writer_head - 1 + frame_buf->length) % frame_buf->length;
    // while (pthread_mutex_trylock(&(frame_buf->buf_locks[read_head])))
    // {
    //      read_head = (frame_buf->writer_head - 1 + frame_buf->length) % frame_buf->length;
    // }
    memcpy(frame, frame_buf->frames + read_head * frame_buf->frame_size, frame_buf->frame_size);
}
