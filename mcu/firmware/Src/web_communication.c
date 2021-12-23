#include "main.h"
#include "web_communication.h"

#include <stdio.h>
#include <stdint.h>

extern UART_HandleTypeDef huart2;

/* Set this to 1 to enable UART logging */
#define WEB_COMMUNICATION_DEBUG 0

#define MAX_MSG_LEN_BYTES 2U
#define END_OF_MSG '\n'

#define MAX_ACTIONS 16U

/* This byte is filled in UART interrupts */
static uint8_t uart_rx_byte;

/* Array for actions */
static struct web_action web_actions[MAX_ACTIONS];
static int registered_actions;

/* Array to input current message */
static uint8_t input_msg[MAX_MSG_LEN_BYTES];
static int input_bytes;

/* Answers to web app */
static const char ok_answer[] = "OK";
static const char er_answer[] = "ER";
static const char not_found[] = "NF";

void webcomm_start_listener(void)
{
	/* Start UART in non-blocking mode */
	HAL_UART_Receive_IT(&huart2, &uart_rx_byte, 1);
}

enum web_action_status webcomm_register_action(const struct web_action *act)
{
	if (registered_actions >= MAX_ACTIONS) {
		return WEBCOMM_ACTION_FAILURE;
	}

	web_actions[registered_actions].pattern = act->pattern;
	web_actions[registered_actions].type = act->type;
	web_actions[registered_actions].action_fn = act->action_fn;

	++registered_actions;

	return WEBCOMM_ACTION_SUCCESS;
}

enum web_action_status webcomm_handle_rx_handler(void)
{
	enum web_action_status res = WEBCOMM_ACTION_FAILURE;

	if (input_bytes > MAX_MSG_LEN_BYTES) {
		/* Cancel this message because of too long! */
		input_bytes = 0;
		HAL_UART_Receive_IT(&huart2, &uart_rx_byte, 1);
		return WEBCOMM_ACTION_FAILURE;
	}

	if (uart_rx_byte == END_OF_MSG) {
		int action_found_flag = 0;

		#if (WEB_COMMUNICATION_DEBUG == 1)
			int n;
			char test[128];

			n = sprintf(test, "pattern = %c\r\ntype=%c\r\n", input_msg[0], input_msg[1]);
			HAL_UART_Transmit(&huart2, (uint8_t *)test, n, 100);
		#endif

		for (int i = 0; i < registered_actions; ++i) { /* go throuth all registered actions */
			if (input_msg[0] == web_actions[i].pattern &&
					input_msg[1] == web_actions[i].type) {

				action_found_flag = 1;
				res = web_actions[i].action_fn(web_actions[i].params);

				if (res == WEBCOMM_ACTION_SUCCESS) {
					HAL_UART_Transmit(&huart2, (uint8_t *)ok_answer,
									sizeof(ok_answer) - 1, 100);
				} else {
					HAL_UART_Transmit(&huart2, (uint8_t *)er_answer,
									sizeof(er_answer) - 1, 100);
				}
			}
		}

		/* reset input array */
		input_bytes = 0;

		if (action_found_flag == 0) {
			res = WEBCOMM_ACTION_NOTFOUND;
			HAL_UART_Transmit(&huart2, (uint8_t *)not_found,
							  sizeof(not_found) - 1, 100);
		}
	} else {
		/* save byte to local buffer */
		input_msg[input_bytes] = uart_rx_byte;
		++input_bytes;
	}

	HAL_UART_Receive_IT(&huart2, &uart_rx_byte, 1);
	return res;
}
