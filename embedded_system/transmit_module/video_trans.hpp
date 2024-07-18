// trans -> transmission
#include <stddef.h>
#include <netinet/in.h>
#include <stdbool.h>
#include "frame.h"
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
 * @brief Initialize the upload module
 * @param connect_timeout: long; wait time for connection, if exceeded, return. Value means: -1 for default(10L), >0 for user defined
 * @param trans_timeout: long; wait time for transmission, if exceeded, return. Value means: -1 for default(10L), >0 for user defined
 * @param token: const char *; token for authentication. Value means: NULL for default, not NULL for user defined
 * @param folder_path: const char *; path to save the uploaded files. Value means: NULL for default, not NULL for user defined
 * @return nothing
*/
void upload_init(long connect_timeout,long trans_timeout,const char *token,const char *folder_path);
/** 
 * @brief Upload a file to the server
 * @param args: char **   | args[0] = &file_id, args[1] = &token
 * @return bool
*/
void *upload_file(void *args);




#ifdef __cplusplus
}
#endif

#endif