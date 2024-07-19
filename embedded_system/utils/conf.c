#include "conf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

/**
 * @brief 从文件中读取并解析服务器的 IP 和端口
 * 
 * @param conf_path 要读取的文件名
 * @return int 0 表示成功，非 0 表示失败
 */
int read_conf(conf_t *conf, const char *conf_path) {
    FILE *file = fopen(conf_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // 解析 server_ip
        if (strncmp(line, "server_ip:", 10) == 0) {
            strncpy(conf->server_ip, line + 11, sizeof(conf->server_ip) - 1);
            conf->server_ip[sizeof(conf->server_ip) - 1] = '\0';
        }
        // 解析 server_port
        else if (strncmp(line, "server_port:", 12) == 0) {
            conf->server_port = atoi(line + 13);
        }
    }

    fclose(file);
    return 0;
}

