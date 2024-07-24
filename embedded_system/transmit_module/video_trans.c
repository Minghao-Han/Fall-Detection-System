// video_trans.c
#include "video_trans.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ostream>
#include <iostream>

// Define constants
#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_CHANNELS 3

// Define global variables
int sockfd = -1;
static int TRANS_CONTiNUE = 0;

// Function to initialize the video stream
stream_t *stream_init(struct sockaddr_in *server_info, frame_buf_t *camera_buf,size_t frame_size) {
    stream_t *stream_ptr = (stream_t *)malloc(sizeof(stream_t));
    if (stream_ptr == NULL) {
        perror("Failed to allocate memory for stream_t");
        return NULL;
    }

    stream_ptr->server_info = server_info;

    // Allocate buffer for camera frames
    stream_ptr->camera_buf = camera_buf;
    if (stream_ptr->camera_buf == NULL) {
        perror("Failed to allocate memory for camera buffer");
        free(stream_ptr);
        return NULL;
    }
    stream_ptr->frame_size = frame_size;
    TRANS_CONTiNUE = 1;
    return stream_ptr;
}

// Function to start the stream module
void *stream_start(void *args) {
    stream_t *stream_ptr = (stream_t *)args;
    // struct sockaddr_in server_addr = *(stream_ptr->server_info);
    const char* command = "ffmpeg -y -f rawvideo -vcodec rawvideo -s 640x480 -pix_fmt nv12 -r 30 -i - -an -c:v libx264 -pix_fmt yuv420p -f flv rtmp://192.168.43.133:1935/live/streamkey";
    
    // 打开一个管道用于写入数据到FFmpeg
    FILE* pipe = popen(command, "w");
    if (!pipe) {
        std::cerr << "Failed to open pipe" << std::endl;
        return NULL;
    }
    
    // int width = 640;
    // int height = 480;
    // int frame_size = width * height * 3 / 2;  // NV12格式，Y分量 + UV分量 (2 planes)
    // unsigned char* frame = new unsigned char[frame_size];
    size_t frame_size = stream_ptr->frame_size;
    frame_t *frame = (frame_t *)malloc(frame_size);
    frame_buf_t *frame_buf = stream_ptr->camera_buf;
    while(TRANS_CONTiNUE){
        frame_buf_pop(frame_buf, frame);
        // 写入一帧数据
        fwrite(frame, 1, frame_size, pipe);
    }
    // 初始化frame数据（例如，设置为黑色帧）
    // memset(frame, 0, frame_size);
    
    
    // 关闭管道
    pclose(pipe);
    
    free(frame);
    return NULL;
}

// Function to destroy the stream
int stream_destroy(stream_t *stream_ptr) {
    TRANS_CONTiNUE = 0;
    if (stream_ptr) {
        if (stream_ptr->camera_buf) {
            free(stream_ptr->camera_buf);
        }
        free(stream_ptr);
    }

    return 0;
}
