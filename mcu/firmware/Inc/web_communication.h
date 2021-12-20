#ifndef __WEB_COMMUNICATION_H
#define __WEB_COMMUNICATION_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

enum web_action_status {
    WEBCOMM_ACTION_SUCCESS,
    WEBCOMM_ACTION_FAILURE,
    WEBCOMM_ACTION_NOTFOUND,
};

struct web_action {
    char pattern;
    char type;
    enum web_action_status (*action_fn)(void *params);
    void *params;
};

void webcomm_start_listener(void);
enum web_action_status webcomm_register_action(const struct web_action *act);
enum web_action_status webcomm_handle_rx_handler(void);


#ifdef __cplusplus
}
#endif

#endif /* __WEB_COMMUNICATION_H */
