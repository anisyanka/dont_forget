#ifndef __WEB_COMMUNICATION_H
#define __WEB_COMMUNICATION_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

#define WEBCOMM_MAX_ANSWER_LEN 16U

enum web_action_status {
    WEBCOMM_ACTION_SUCCESS,
    WEBCOMM_READ_ANSWER_FROM_PARAMS,
    WEBCOMM_ACTION_FAILURE,
    WEBCOMM_ACTION_NOTFOUND,
};

struct web_action_params {
    // buffer to save answer for 'get_status' type of command
    uint8_t answer[WEBCOMM_MAX_ANSWER_LEN];
    size_t answer_len;
    void *users_param;
};

struct web_action {
    char pattern;
    char type;
    enum web_action_status (*action_fn)(void *users_param);
    struct web_action_params *params;
};

void webcomm_start_listener(void);
enum web_action_status webcomm_register_action(const struct web_action *act);
enum web_action_status webcomm_handle_rx_handler(void);


#ifdef __cplusplus
}
#endif

#endif /* __WEB_COMMUNICATION_H */
