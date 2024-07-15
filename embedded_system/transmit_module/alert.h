#ifndef _ALERT_H
#define _ALERT_H 1
#include <netinet/in.h>

// void alert_init();
/**
 * @brief Send an alert to the server
 * @param server_info: struct sockaddr_in *
 * @return void
 */
void send_alert(struct sockaddr_in *server_info);


#endif // _ALERT_H