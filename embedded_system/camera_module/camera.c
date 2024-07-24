#include "camera.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include <sp_sys.h>
#include <sp_display.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <unistd.h>
#include <sys/stat.h>
#include <semaphore.h>

static uint8_t camera_continue = 0;
static sem_t camera_exited;


double get_stride(int width, int bit)
{
    double temp = (width * bit / 8.0 / 16.0); // Determining whether alignment is possible
    double fractpart;
    double intpart;
    double result;
    // get fractpart
    fractpart = modf(temp, &intpart);

    if (fractpart > 0)
    {
        // Rounding
        return ceil(temp) * 16;
    }

    return temp * 16;
}

static char doc[] = "capture sample -- An example of capture yuv/raw";

camera_t *camera_init(sp_sensors_parameters *camera_params,int channel_num,frame_buf_t *camera_buf){
    camera_t *camera = (camera_t *)malloc(sizeof(camera_t));
    int32_t yuv_width=camera->yuv_width = camera_params->raw_width;
    int32_t yuv_height=camera->yuv_height = camera_params->raw_height;

    int yuv_size = FRAME_BUFFER_SIZE(yuv_width, yuv_height);
    camera->frame_size = yuv_size;
    camera->camera_buf = camera_buf;

    char ch = 0;
    int is_enter = 0;
    int yuv_count = 0;
    int raw_count = 0;
    char yuv_filename[50];
    // init camera
    void *camera_prt = sp_init_vio_module();
    camera->vio_camera_ptr = camera_prt;
    // open camera
    int ret = sp_open_camera_v2(camera_prt, 3, -1, 1, camera_params,&yuv_width, &yuv_height);
    printf("wait for isp stability\n");
    sleep(2); // wait for isp stability
    printf("open camera ret:%d\n", ret);
    if (ret != 0)
    {
        printf("[Error] sp_open_camera failed!\n");
        sp_vio_close(camera_prt);
        sp_release_vio_module(camera_prt);
        return NULL;
    }
    sem_init(&camera_exited, 0, 0);
    return camera;
}


void *camera_start(void *args){
    camera_t *camera = (camera_t *)args;
    frame_buf_t *camera_buf = camera->camera_buf;
    int yuv_count = 0;
    camera_continue=1;
    int yuv_width = camera->yuv_width;
    int yuv_height = camera->yuv_height;
    int yuv_size = camera->frame_size;
    char *yuv_data = (char *)malloc(yuv_size * sizeof(char));
    int16_t buf_len = camera_buf->length;
    void *camera_prt = camera->vio_camera_ptr;
    char yuv_filename[]="/home/sunrise/clip/right.yuv";
    int16_t writer_head;
    do
    {
        // printf("capture time :%d\n", yuv_count);
        // printf("camera_buf %p\n",yuv_data);
        sp_vio_get_yuv(camera_prt, yuv_data, yuv_width, yuv_height, 2000);
        writer_head = camera_buf->writer_head;
        pthread_mutex_lock((camera_buf->buf_locks)+writer_head);
        memcpy(camera_buf->frames + writer_head * yuv_size, yuv_data, yuv_size);
        pthread_mutex_unlock((camera_buf->buf_locks)+writer_head);
        camera_buf->writer_head = (writer_head + 1) % buf_len;
        // FILE *yuv_file = fopen(yuv_filename, "wb");
        // fwrite(yuv_data, sizeof(char), yuv_size, yuv_file);
        // fflush(yuv_file);
        // fclose(yuv_file); //?需要吗
        // yuv_count++;
    }while (camera_continue);// here
    sem_post(&camera_exited);
    free(yuv_data);
    
}
void camera_destroy(camera_t *camera){
    camera_continue=0;
    sem_wait(&camera_exited);
    sp_vio_close(camera->vio_camera_ptr);
    sp_release_vio_module(camera->vio_camera_ptr);
    printf("camera destroy before\n");
    free(camera);
}