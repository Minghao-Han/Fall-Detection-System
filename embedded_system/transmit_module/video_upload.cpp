#include <iostream>
#include <curl/curl.h>
#include <fstream>

// 定义上传文件的URL,到时候要改动的。 replace localhost to server's ip
#define UPLOAD_URL "http://localhost:8080/video/upload"

// 上传文件的函数
bool uploadFile(const char* filePath) {
    CURL *curl;
    CURLcode res;
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    struct curl_slist *headerlist = NULL;
    static const char buf[] = "Expect:";

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
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

        // 提交请求
        res = curl_easy_perform(curl);

        // 检查请求是否成功
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            return false;
        }

        // 清理curl和表单
        curl_easy_cleanup(curl);
        curl_formfree(formpost);

        return true;
    }
    return false;
}

