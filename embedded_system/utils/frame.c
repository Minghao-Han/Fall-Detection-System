#include "frame.h"

frame_t *frame_buf_init(size_t width, size_t height, int channel_num){
    return (frame_t *)malloc(width * height * channel_num);
}