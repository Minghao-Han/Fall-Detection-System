#include "clip.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <string.h>

#define DEFAULT_CLIP_SAVER_META_INFO_PATH "/meta_info.txt"

int clip_saver_init(clip_saver_t *clip_saver, int capacity, char *folder_path,frame_buf_t *clip){
    clip_saver_t *clip_saver = (clip_saver_t *)malloc(sizeof(clip_saver_t));
    if (clip_saver == NULL)
    {
        return 1;
    }
    clip_saver->capacity = capacity;
    clip_saver->clip_num = 0;
    clip_saver->current_ptr = 1;
    clip_saver->clip = clip;
    clip_saver->folder_path = folder_path;
    return 0;
}

int save_clip(clip_saver_t *clip_saver){
    char filename[6]; // 保存文件名的字符串     /{index}.mp4   
    char new_file_name[64] = clip_saver->folder_path; // 注意，该字符串的大小可能是导致缓冲区溢出/segment fault的原因，但是这样是为了减少内存使用。
    
    if (clip_saver->clip_num > clip_saver->capacity)
    {
        perror("The number of clips exceeds the capacity");
        exit(EXIT_FAILURE);
        return -1;
    }
    int file_index=clip_saver->current_ptr;
    // 使用 sprintf 函数格式化字符串并保存到 filename 中
    sprintf(filename, "/%d.mp4", file_index);
    strcat(new_file_name, filename);

    // Open the new file for writing, if exists, overwrite it
    FILE *new_file = fopen(new_file_name, "w");
    // if the file is not opened successfully, print the error message and return
    if (new_file == NULL) {
        perror("Failed to open the new file\n");
        return -1;
    }
    
    int current=clip_saver->clip->current_ptr;
    size_t frame_size=clip_saver->clip->frame_size;
    frame_t *start=clip_saver->clip->frames;
    int length=clip_saver->clip->length;

    // Write the video clips from memory to the new file
    // 写入 buffer[current_ptr:end] 部分到文件
    fwrite(start+current*frame_size, frame_size, length-current+1, new_file);

    // 写入 buffer[start:current_ptr] 部分到文件
    fwrite(start, frame_size, current, new_file);

    // Close the new file
    fclose(new_file);
    clip_saver->current_ptr++;
    clip_saver->clip_num++;
    return file_index;
}

void clip_saver_close(clip_saver_t *clip_saver)
{
    FILE *meta_info = fopen(DEFAULT_CLIP_SAVER_META_INFO_PATH, "w");
    // remain to be implemented
}
