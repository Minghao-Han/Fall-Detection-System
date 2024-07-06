// trans -> transmission
#include <stddef.h>
#include <netinet/in.h>
#ifndef _VIDEO_TRANS_HPP
#define _VIDEO_TRANS_HPP 1

#ifdef __cplusplus
extern "C"{
#endif

typedef struct stream_t{}stream_t;
int stream_init(stream_t *stream,struct sockaddr_in *server_info, void *camera_buf, size_t len);
int stream_start(stream_t *stream_ptr);
int stream_destroy(stream_t *stream_ptr);
bool uploadFile(const char* filePath);


#ifdef __cplusplus
}
#endif

#endif