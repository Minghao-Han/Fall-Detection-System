#include "alert.h"
#include <iostream>
#include <curl/curl.h>

#define UPLOAD_URL "http://10.128.149.159:8080/api/alerts/receive"

void send_alert(const char* token) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL); // 初始化 libcurl

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, UPLOAD_URL);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "Warning! Elderly fell!");
        
        // 如果需要额外配置，例如 HTTP 头部，可以在此设置
        struct curl_slist *headers = NULL;
        std::string authHeader = "Token: ";
        authHeader += token;
        headers = curl_slist_append(headers, authHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() FAILED: " << curl_easy_strerror(res) << std::endl;
        }
        
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers); // 释放头部链表
    }

    curl_global_cleanup(); // 清理 libcurl 资源
}

// int main(int argc, char *argv[]) {
//     if(argc < 2) {
//         std::cerr << "Usage: " << argv[0] << " <token>" << std::endl;
//         return 1;
//     }
//     const char* token = argv[1];
//     send_alert(token);
//     return 0;
// }

