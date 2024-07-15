// trans -> transmission
#include <stddef.h>
#include <netinet/in.h>
#include <stdbool.h>
#ifndef _VIDEO_TRANS_HPP
#define _VIDEO_TRANS_HPP 1

#ifdef __cplusplus
extern "C"{
#endif

typedef struct stream_t{
    struct sockaddr_in *server_info;
    void *camera_buf;
    size_t frame_size; // default shoud be 640*480*3;
}stream_t;

stream_t *stream_init(struct sockaddr_in *server_info, frame_t *camera_buf, size_t frame_size);

/** 
 * @brief Start the stream module
 * @param stream_ptr: stream_t *
 * @return nothing
*/
void *stream_start(void *args);
int stream_destroy(stream_t *stream_ptr);

/** 
 * @brief Upload a file to the server
 * @param file_index: int file index in clip folder, you need to concatenate the file index with the folder path to get the full file path
 * @return bool
*/
void *upload_file(void *args);


#ifdef __cplusplus
}
#endif

#endif