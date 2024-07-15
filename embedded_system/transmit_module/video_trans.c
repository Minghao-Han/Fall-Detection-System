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
#define SERVER_PORT 8080

// Define global variables
int sockfd = -1;

// Function to initialize the video stream
stream_t *stream_init(struct sockaddr_in *server_info, void *camera_buf, size_t frame_size) {
    stream_t *stream_ptr = (stream_t *)malloc(sizeof(stream_t));
    if (stream_ptr == NULL) {
        perror("Failed to allocate memory for stream_t");
        return NULL;
    }

    stream_ptr->server_info = server_info;

    // Allocate buffer for camera frames
    stream_ptr->camera_buf = malloc(frame_size);
    if (stream_ptr->camera_buf == NULL) {
        perror("Failed to allocate memory for camera buffer");
        free(stream_ptr);
        return NULL;
    }
    memcpy(stream_ptr->camera_buf, camera_buf, frame_size);
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

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error: Connection failed");
        return NULL;
    }

    // Send frames to server
    while (1) {
        // Simulate sending camera frames (replace with actual camera capturing logic)
        usleep(10000);  // Simulate 10ms delay

        // Send camera_buf to server (replace with actual sending logic)
        if (send(sockfd, stream_ptr->camera_buf, stream_ptr->frame_size, 0) != stream_ptr->frame_size) {
            perror("Error: Failed to send frame");
            break;
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
