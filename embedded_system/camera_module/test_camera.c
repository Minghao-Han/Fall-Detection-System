#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "camera.h"
#include <stdbool.h>

#define CAMERA_THREAD 0
#define FALL_DETECTION_THREAD 1
#define VIDEO_STREAM 2
#define ALERT_THREAD 3 
#define SEND_CLIP_THREAD 4

#define DEFAULT_CONF_PATH "./conf.txt"

static const frame_t *camera_buf;
static bool continue_loop = true;

void signal_handler_func(int signum)
{
    printf("\nrecv:%d,Stoping...\n", signum);
    continue_loop = false;
}

int main(){
    // set frame specs | read from config
    const size_t width=1080;
    const size_t height=1920;
    const int fps=30; // frame per second
    const int channel_num=3;


    pthread_t threads[5];
    // camera init and start
    camera_t *camera = camera_init(width,height,channel_num);
    camera_buf=camera->camera_buf;
    pthread_create(&threads[CAMERA_THREAD], NULL, &camera_start, (void *)camera);
    
    signal(SIGINT, signal_handler_func);
    pthread_detach(threads[CAMERA_THREAD]);
    while (continue_loop);
    
    camera_destroy(camera);
    return 0;
}