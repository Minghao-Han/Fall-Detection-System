#include <iostream>
#include <curl/curl.h>
#include <fstream>
#include <cstring>
#include "video_trans.hpp"

static const char *FOLDER_PATH = "/home/sunrise/clip";
static const char *TOKEN="eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJhdWQiOiIxNTUyNzY4NTA3MCIsImV4cCI6MTcyMTExMjcwNn0.yliAQoQkqNf02LPTL31yRpt2I8dcPNi8IgjTchYIEkI";
static long CONNECT_TIMEOUT=10L;
static long TRANS_TIMEOUT=20L;

#define UPLOAD_URL "http://10.128.149.159:8080/api/video/upload"

void *upload_file(void *args) {
    const int clip_id = (int)((long)args);
    char filename[6]; // 保存文件名的字符串     /{index}.mp4   
    char filePath[64] = {0}; // Initialize the array with a brace-enclosed initializer
    strcat(filePath, FOLDER_PATH);
    // 使用 sprintf 函数格式化字符串并保存到 filename 中
    sprintf(filename, "/%d.yuv", clip_id);
    strcat(filePath, filename);
    CURL *curl;
    CURLcode res;
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    struct curl_slist *headerlist = NULL;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    printf("Uploading file %s\n", filePath);
    if(curl) {
        // 添加上传文件字段
        curl_formadd(&formpost,
                     &lastptr,
                     CURLFORM_COPYNAME, "file",
                     CURLFORM_FILE, filePath,
                     CURLFORM_END);

        // 设置curl选项
        curl_easy_setopt(curl, CURLOPT_URL, UPLOAD_URL);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        // 设置连接超时为10秒
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, CONNECT_TIMEOUT);
        // 设置整个请求操作的超时为20秒
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, TRANS_TIMEOUT);

        // 添加Token头部
        std::string authHeader = "Token: ";
        authHeader += TOKEN;
        headerlist = curl_slist_append(headerlist, authHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

        // 提交请求
        res = curl_easy_perform(curl);

        // 检查请求是否成功
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            return nullptr;
        }

        // 清理curl和表单
        curl_easy_cleanup(curl);
        curl_formfree(formpost);
        curl_slist_free_all(headerlist);

        fprintf(stdout, "File uploaded successfully!\n");
        return nullptr;
    }
    return nullptr;
}

void upload_init(long connect_timeout,long trans_timeout,const char *token,const char *folder_path){
    CONNECT_TIMEOUT=(connect_timeout+1)?connect_timeout:CONNECT_TIMEOUT;
    TRANS_TIMEOUT=(trans_timeout+1)?trans_timeout:TRANS_TIMEOUT;
    TOKEN=token?token:TOKEN;
    FOLDER_PATH=folder_path?folder_path:FOLDER_PATH;
} 

// int main(int argc, char *argv[]) {
//     if(argc < 3) {
//         std::cerr << "Usage: " << argv[0] << " <file_path_to_upload> <token>" << std::endl;
//         return 1;
//     }

//     const char* filePath = argv[1];
//     const char* token = argv[2];

//     // 调用上传文件函数
//     if(uploadFile(filePath, token)) {
//         std::cout << "File uploaded successfully!" << std::endl;
//     } else {
//         std::cerr << "Failed to upload file." << std::endl;
//     }

//     return 0;
// }

