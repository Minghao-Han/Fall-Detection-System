#include "frame.h"


frame_buf_t *frame_buf_init(size_t width, size_t height,int channel_num, size_t length){
    frame_buf_t *frame_buf = (frame_buf_t *)malloc(sizeof(frame_buf_t));
    if(frame_buf == NULL){
        return NULL;
    }
    frame_buf->frame_size=width * height * channel_num;
    frame_buf->length = length;
    frame_buf->current_ptr = 0;
    frame_buf->frames = (frame_t *)malloc(sizeof(frame_t) * width * height * channel_num * length);
    if(frame_buf->frames == NULL){
        free(frame_buf);
        return NULL;
    }
    return frame_buf;
}
void frame_buf_destroy(frame_buf_t *frame_buf){
    free(frame_buf->frames);
    free(frame_buf);
}
void frame_buf_push(frame_buf_t *frame_buf, frame_t *frame){
    memcpy(frame_buf->frames + frame_buf->current_ptr * frame_buf->frame_size, frame, frame_buf->frame_size);
    frame_buf->current_ptr = (frame_buf->current_ptr + 1) % frame_buf->length;
}