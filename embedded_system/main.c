#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <stddef.h>
#include "camera_module/camera.h"
#include "sp_vio.h"
#include "sp_sys.h"
#include "fall_detection.h"
#include "alert.h"
#include "clip.h"
#include "conf.h"
#include "video_trans.hpp"
#define TOKEN_TEMP "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJhdWQiOiIxNTUyNzY4NTA3MCIsImV4cCI6MTcyMTExMjcwNn0.yliAQoQkqNf02LPTL31yRpt2I8dcPNi8IgjTchYIEkI"

#define CAMERA_THREAD 0
#define FALL_DETECTION_THREAD 1
#define VIDEO_STREAM 2
#define ALERT_THREAD 3 
#define SEND_CLIP_THREAD 4

#define DEFAULT_CONF_PATH "./conf.txt"

pthread_mutex_t camera_buf_lock;

static frame_t *camera_buf;
static bool continue_loop = true;
static sem_t fall_sem;

void signal_handler_func(int signum)
{
    printf("\nrecv:%d,Stoping...\n", signum);
    continue_loop = false;
    sem_post(&fall_sem);
}

void clean_up(camera_t *camera,fall_detector_t *fall_detector,frame_buf_t *frame_buf,clip_saver_t *clip_saver,stream_t *stream){
    // free fall detector
    if(fall_detector){
        printf("cleaning up detector ");
        fall_detector_destroy(fall_detector);
        printf(" done\n");
    }
    // free clip
    if(frame_buf){
        printf("cleaning up clip ");
        frame_buf_destroy(frame_buf);
        printf(" done\n");
    }
    // free stream
    // if(stream)stream_destroy(stream);
    // free camera
    if(camera){
        printf("cleaning up camera ");
        camera_destroy(camera);
        printf(" done\n");
    }
    printf("clean up completed.\n");
}

int main(){
    // read config file
    conf_t config;
    if(read_conf(&config,DEFAULT_CONF_PATH) == 1){
        perror("read config file error");
        exit(EXIT_FAILURE);
    }
    config.clip_folder = "/home/sunrise/clip";
    sp_sensors_parameters sensor_params;
    // set frame specs | should read from config
    sensor_params.fps=30;
    sensor_params.raw_width=640;
    sensor_params.raw_height=480;
    const int fps=30; // frame per second
    const int channel_num=0;

    int8_t frame_len_seconds=7;
    int16_t frame_buf_length = frame_len_seconds*fps; // can store 7 seconds 
    frame_buf_t *frame_buf = frame_buf_init(sensor_params.raw_width,sensor_params.raw_height,channel_num, frame_buf_length);


    pthread_t threads[5];
    // camera init and start
    camera_t *camera = camera_init(&sensor_params,channel_num,frame_buf);
    camera->camera_buf=frame_buf;
    pthread_create(&threads[CAMERA_THREAD], NULL, &camera_start, (void *)camera);
    printf("wait %d seconds for camera to fullfill the whole buf\n",frame_len_seconds);
    sleep(frame_len_seconds); //wait 7 seconds so that camera can fullfill the whole buf
    printf("continue initializing\n");
    // video stream transmission init and start
    struct sockaddr_in server_info;//定义sockaddr_in结构体mysock
 
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    /******填充sockaddr_in******/
    bzero(&server_info,sizeof(server_info));  //初始化结构体
    server_info.sin_family = AF_INET;  //设置地址家族 IPV4
    server_info.sin_port = htons(atoi(config.server_ip));  //设置端口
    // 将 IP 地址从字符串转换为网络字节序，并设置到结构体中
    if (inet_pton(AF_INET, config.server_ip, &server_info.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(EXIT_FAILURE);
    }
    stream_t *stream = stream_init(&server_info, (void *)camera_buf, sizeof(camera_buf));
    pthread_create(&threads[VIDEO_STREAM], NULL, &stream_start, (void *)stream);

    // init clip and its saver 
    int clip_length = 5;    // 5 seconds per clip
    
    clip_saver_t clip_saver;
    if (clip_saver_init(&clip_saver, 10, config.clip_folder, frame_buf,clip_length*fps))
    {
        perror("inet_pton error");
        exit(EXIT_FAILURE);
    }
    

    // fall detection init and start
    // init sem for fall detection, set initial value to 0
    sem_init(&fall_sem, 0, 0);
    fall_detector_t *fall_detector = fall_detector_init(&fall_sem, frame_buf);
    pthread_create(&threads[FALL_DETECTION_THREAD], NULL, &fall_detector_start, (void *)fall_detector);
    
    upload_init(-1,-1,NULL,NULL);

    signal(SIGINT, signal_handler_func);
    printf("initialization completed.\n");
    while (continue_loop)
    {
        // wait for fall detection ｜ 等待跌倒检测
        sem_wait(&fall_sem);
        // break the loop if the program is going to stop ｜ 如果程序即将停止，则退出循环
        if (!continue_loop) break;

        // alert
        // send_alert(TOKEN_TEMP);
        int file_idx=save_clip(&clip_saver);
        printf("fell. clip saved with index %d\n",file_idx);

        // send the clip to the server in a new thread in order to not slow down the main thread ｜ 为了不拖慢主线程，将剪辑发送到服务器的操作放在一个新线程中
        // convert file_idx to void * type so that it can be passed to the thread function ｜ 将 file_idx 转换为 void * 类型，以便将其传递给线程函数
        pthread_create(threads+SEND_CLIP_THREAD, NULL, &upload_file, (void *)(long)file_idx); 
        pthread_detach(threads[SEND_CLIP_THREAD]);
    }
    
    clean_up(camera,fall_detector,frame_buf,&clip_saver,NULL);
    printf("program exit.\n");
    return 0;
}