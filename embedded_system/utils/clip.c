#include "clip.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <string.h>

#define DEFAULT_CLIP_SAVER_META_INFO_PATH "/home/sunrisemeta_info.txt"

int clip_saver_init(clip_saver_t *clip_saver, int capacity, char *folder_path,frame_buf_t *frame_buf,int16_t clip_length){
    if (clip_saver == NULL)
    {
        return 1;
    }
    DIR *dir = opendir(folder_path);
    if (dir == NULL) {
        perror("Failed to open the folder\n");
        return 1;
    }
    struct dirent *entry;
    int file_count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // check if the entry is a regular file
            file_count++;
        }
    }
    closedir(dir);
    clip_saver->capacity = capacity;
    clip_saver->clip_length = clip_length;
    clip_saver->clip_num = file_count;
    clip_saver->current_file_idx = 0;
    clip_saver->frame_buf = frame_buf;
    clip_saver->folder_path = folder_path;
    return 0;
}

int save_clip(clip_saver_t *clip_saver){
    char filename[6]; // 保存文件名的字符串     /{index}.mp4   
    char new_file_name[64] = {0}; // Initialize the array with a brace-enclosed initializer
    strcat(new_file_name, clip_saver->folder_path);
    
    if (clip_saver->clip_num > clip_saver->capacity)
    {
        perror("The number of clips exceeds the capacity");
        exit(EXIT_FAILURE);
        return -1;
    }
    int file_index=clip_saver->current_file_idx;
    // 使用 sprintf 函数格式化字符串并保存到 filename 中
    sprintf(filename, "/%d.yuv", file_index);
    strcat(new_file_name, filename);

    // Open the new file for writing, if exists, overwrite it
    FILE *new_file = fopen(new_file_name, "w");
    // if the file is not opened successfully, print the error message and return
    if (new_file == NULL) {
        perror("Failed to open the new file\n");
        return -1;
    }
    
    int16_t current=clip_saver->frame_buf->writer_head;
    size_t frame_size=clip_saver->frame_buf->frame_size;
    frame_t *buf_start=clip_saver->frame_buf->frames;
    int16_t length=clip_saver->frame_buf->length;
    int16_t clip_length=clip_saver->clip_length;
    int16_t clip_length_half=(clip_length)>>1;//clip_length_half=clip_length/2

    int16_t clip_end=(current+clip_length_half)%length; //clip的结束位置
    int16_t clip_start=(current-clip_length_half+length)%length; //clip的开始位置
    if (clip_end<clip_start) // bf|******e------s******|be
    {
        int16_t *current=&(clip_saver->frame_buf->writer_head);
        printf("from %d buf_end and from buf_start to %d\n",clip_start,clip_end);
        while ((*current<=clip_end)||(*current>=clip_start)); //现在摄像头读写的buf会是正在保存的，会对正在保存的clip造成影响，while循环等待
        fwrite(buf_start+clip_start*frame_size, frame_size, length-clip_start, new_file);
        fwrite(buf_start, frame_size, clip_end, new_file);
    }else{// bf|------s*******e------|be
        printf("from %d to %d\n",clip_start,clip_end);
        int16_t *current=&(clip_saver->frame_buf->writer_head);
        while ((*current<=clip_end)&&(*current>=clip_start));//现在摄像头读写的buf是正在保存的，会对正在保存的clip造成影响，while循环等待
        fwrite(buf_start+clip_start*frame_size, frame_size, clip_length, new_file);
    }
    fclose(new_file);
    clip_saver->current_file_idx++;
    clip_saver->clip_num++;
    return file_index;
}

void clip_saver_close(clip_saver_t *clip_saver)
{
    FILE *meta_info = fopen(DEFAULT_CLIP_SAVER_META_INFO_PATH, "w");
    // remain to be implemented
}
