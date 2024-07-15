#include <iostream>
#include <curl/curl.h>
#include <fstream>

#define UPLOAD_URL "http://10.128.149.159:8080/api/video/upload"

bool uploadFile(const char* filePath, const char* token) {
    CURL *curl;
    CURLcode res;
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    struct curl_slist *headerlist = NULL;

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

        // 添加Token头部
        std::string authHeader = "Token: ";
        authHeader += token;
        headerlist = curl_slist_append(headerlist, authHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

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
        curl_slist_free_all(headerlist);

        return true;
    }
    return false;
}

int main(int argc, char *argv[]) {
    if(argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <file_path_to_upload> <token>" << std::endl;
        return 1;
    }

    const char* filePath = argv[1];
    const char* token = argv[2];

    // 调用上传文件函数
    if(uploadFile(filePath, token)) {
        std::cout << "File uploaded successfully!" << std::endl;
    } else {
        std::cerr << "Failed to upload file." << std::endl;
    }

    return 0;
}

