#ifndef _CLIP_H
#define _CLIP_H 1

#include <stddef.h>
#include "frame.h"

static const char *DEFAULT_CLIP_FOLDER = "/home/sunrise/clip/";

// If the size of the clip exceeds the capacity defined in clip_saver_t, it will overwrite the oldest clip. ｜ 当clip的大小超过clip_saver_t中定义的capacity时，将覆盖最老的clip
typedef struct clip_saver_t { //a saver saving clip to tf card ｜ 一个将clip保存到硬盘的保存器
    int capacity;    //define how many clips can be save in tf card at most at one time  ｜ 硬盘中最多可以存储多少个clip
    int clip_num;    //current clip number in tf card ｜ 硬盘中当前存储的clip数量
    int current_file_idx; //current clip pointer ｜ 当前clip指针
    int16_t clip_length;   //length of a clip, should shorter than frame_buf_t.length ｜ clip的长度，应该比frame_buf_t.length短
    frame_buf_t *frame_buf;  // clip to be saved ｜ 要保存的clip
    char *folder_path;         // path to save clip ｜ 保存clip的路径
} clip_saver_t;

/**
 * @brief Initialize a clip_saver_t object
 * @param clip_size: size of a clip
 * @param capacity: define how many clips can be save in tf card at most at one time
 * @param folder_path: path to save clip
 * @param clip: clip to be saved
 * @return 1: failed to initialize the clip_saver_t object 0: success
 */
int clip_saver_init(clip_saver_t *clip_saver, int capacity, char *folder_path, frame_buf_t *frame_buf,int16_t clip_length);

/**
 * @brief Save a clip to tf card
 * @param clip_saver: clip_saver_t *
 * @return -1: failed to save the clip, positive int: success
 */
int save_clip(clip_saver_t *clip_saver);
void clip_saver_close(clip_saver_t *clip_saver);

#endif // _CLIP_H