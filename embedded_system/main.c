#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include "camera.h"
#include "fall_detection.h"
#include "alert.h"
#include "clip.h"
#include "conf.h"
#include "video_trans.hpp"

#define CAMERA_THREAD 0
#define FALL_DETECTION_THREAD 1
#define VIDEO_STREAM 2
#define ALERT_THREAD 3 
#define SEND_CLIP_THREAD 4

#define DEFAULT_CONF_PATH "./conf.txt"

static const frame_t *camera_buf;
static bool continue_loop = true;
static sem_t fall_sem;

void signal_handler_func(int signum)
{
    printf("\nrecv:%d,Stoping...\n", signum);
    continue_loop = false;
    sem_post(&fall_sem);
}

void clean_up(camera_t *camera,fall_detector_t *fall_detector,frame_buf_t *clip,clip_saver_t *clip_saver,stream_t *stream){
    // free fall detector
    fall_detector_destroy(fall_detector);
    // free clip
    frame_buf_destroy(clip);
    // close clip saver
    clip_saver_close(clip_saver);
    // free stream
    stream_destroy(stream);
    // free camera
    camera_destroy(camera);
}

int main(){
    // read config file
    conf_t config;
    if(read_conf(&config,&DEFAULT_CONF_PATH) == 1){
        perror("read config file error");
        exit(EXIT_FAILURE);
    }
    // set frame specs | read from config
    const size_t width=640;
    const size_t height=480;
    const int fps=30; // frame per second
    const int channel_num=3;


    pthread_t threads[5];
    // camera init and start
    camera_t *camera = camera_init(width,height,channel_num);
    camera_buf=camera->camera_buf;
    pthread_create(&threads[CAMERA_THREAD], NULL, &camera_start, (void *)camera);
    // video stream transmission init and start
    struct sockaddr_in server_info;
    // 清空结构体
    memset(&server_info, 0, sizeof(server_info));

    // 设置地址族为 IPv4
    server_info.sin_family = AF_INET;

    // 设置端口号，使用 htons 将主机字节序转换为网络字节序
    server_info.sin_port = htons(config.server_ip);

    // 将 IP 地址从字符串转换为网络字节序，并设置到结构体中
    if (inet_pton(AF_INET, config.server_ip, &server_info.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(EXIT_FAILURE);
    }
    stream_t *stream = stream_init(&server_info, (void *)camera_buf, sizeof(camera_buf));
    pthread_create(&threads[VIDEO_STREAM], NULL, &stream_start, (void *)stream);

    // init clip and its saver 
    int clip_length = 5;    // 5 seconds per clip
    frame_buf_t *clip = frame_buf_init(width,height,channel_num, clip_length * fps);
    clip_saver_t clip_saver;
    if (clip_saver_init(&clip_saver, 10, config.clip_folder, clip))
    {
        perror("inet_pton error");
        exit(EXIT_FAILURE);
    }
    

    // fall detection init and start
    // init sem for fall detection, set initial value to 0
    sem_init(&fall_sem, 0, 0);
    fall_detector_t *fall_detector = fall_detector_init(camera_buf, &fall_sem, clip);
    pthread_create(&threads[FALL_DETECTION_THREAD], NULL, &fall_detector_start, (void *)fall_detector);


    signal(SIGINT, signal_handler_func);

    while (continue_loop)
    {
        // wait for fall detection ｜ 等待跌倒检测
        sem_wait(&fall_sem);
        // break the loop if the program is going to stop ｜ 如果程序即将停止，则退出循环
        if (!continue_loop) break;
        // alert
        send_alert(&server_info);
        int file_idx=save_clip(&clip_saver);
        // send the clip to the server in a new thread in order to not slow down the main thread ｜ 为了不拖慢主线程，将剪辑发送到服务器的操作放在一个新线程中
        // convert file_idx to void * type so that it can be passed to the thread function ｜ 将 file_idx 转换为 void * 类型，以便将其传递给线程函数
        pthread_create(threads+SEND_CLIP_THREAD, NULL, &upload_file, (void *)(long)file_idx); 
        pthread_detach(threads[SEND_CLIP_THREAD]);
    }
    
    clean_up(camera,fall_detector,clip,&clip_saver,stream);
    return 0;
}