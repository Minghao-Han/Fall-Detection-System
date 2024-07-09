#ifndef _CONF_H
#define _CONF_H 1

typedef struct conf_t {
    char *server_ip;
    int server_port; 
    char *clip_folder;
} conf_t;

int read_conf(conf_t *conf, const char *conf_path);

#endif // _CONF_H