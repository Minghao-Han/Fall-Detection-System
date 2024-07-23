// video_trans.c
#include "video_trans.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

// Define constants
#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_CHANNELS 3

// Define global variables
int sockfd = -1;

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

    return stream_ptr;
}

// Function to start the stream module
void *stream_start(void *args) {
    stream_t *stream_ptr = (stream_t *)args;
    struct sockaddr_in server_addr = *(stream_ptr->server_info);
    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error: Failed to create socket");
        return NULL;
    }

    // const char *token = "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJhdWQiOiIxNTUyNzY4NTA3MCIsImV4cCI6MTcyMTQ2NjQyMX0.MmFavYMiIYSArPt93ckAYRgi1rmjdebptVc4p3pIxsY";
    // printf("1\n");
    // if (send(sockfd, token, strlen(token), 0) > 0 ) printf("success\n");
    // if (send(sockfd, token, strlen(token), 0) == 0 ) printf("close\n");
    // if (send(sockfd, token, strlen(token), 0) == -1 ) printf("failed\n");
    // if (send(sockfd, token, strlen(token), 0) != strlen(token)) {
    //     printf("2\n");
    //     perror("发送token失败");
    //     close(sockfd);
    //     return NULL;
    // }
    // printf("3\n");
    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("3\n");
        perror("Error: Connection failed");
        return NULL;
    }
    
    frame_buf_t *camera_buf = stream_ptr->camera_buf;
    size_t frame_size = stream_ptr->frame_size;
    frame_t *frame_ptr;
    // Send frames to server
    printf("The frame size is: %zu bytes\n", frame_size);
    while (1) {
        //printf("1-1");
        // Simulate sending camera frames (replace with actual camera capturing logic)
        usleep(10000);  // Simulate 10ms delay
        printf("-=-");

        // Send camera_buf to server (replace with actual sending logic)
        frame_ptr = camera_buf->frames + camera_buf->writer_head * frame_size - 1;
        ssize_t sent_bytes = send(sockfd, frame_ptr, frame_size, 0);
        if (sent_bytes == -1) {
            printf("Error code: %d\n", errno);
            perror("Error: Failed to send frame");
            break; // Exit the loop on error
        } else if (sent_bytes != frame_size) {
            printf("Warning: Only %ld bytes out of %zu were sent.\n", sent_bytes, frame_size);
            // Decide whether to break or handle partial sends here
        }
    }

    close(sockfd);
    return NULL;
}

// Function to destroy the stream
int stream_destroy(stream_t *stream_ptr) {
    if (stream_ptr) {
        if (stream_ptr->camera_buf) {
            free(stream_ptr->camera_buf);
        }
        free(stream_ptr);
    }

    return 0;
}
