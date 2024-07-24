#ifndef _ALERT_H
#define _ALERT_H 1
#include <netinet/in.h>


#ifdef __cplusplus
extern "C"{
#endif
// void alert_init();
/**
 * @brief Send an alert to the server
 * @param token: const char* token
 * @return void
 */
void send_alert(const char* token);


#ifdef __cplusplus
}
#endif
#endif // _ALERT_H