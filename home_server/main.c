#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define HTTPSERVER_IMPL
#include "httpserver.h"

#define HW_ANSWER_MAX_LEN 16U

struct hw_action {
	char *command;
	char answer[HW_ANSWER_MAX_LEN];
};

enum web_apis_indx {
	ON,
	OFF,
	STATE,
	LAST
};

enum hw_error_codes {
	HW_ACTION_SUCCESS,
	HW_ACTION_FAULURE,
	HW_ACTION_NOT_FOUND,
};

struct web_hw_match_cmd {
	char *web_cmd;
	char *hw_cmd;
};

static struct web_hw_match_cmd web_apis_list[] = {
	[ON] = { "/on", "L1\n" },
	[OFF] = { "/off", "L0\n" },
	[STATE] = { "/state", "LS\n" },
	[LAST] = { NULL, NULL },
};


static int lamp_process_request(struct hw_action *action); /* if succes Returns 'HW_ACTION_SUCCESS' */
static void handle_request(struct http_request_s *request); /* common http requests handler */

int main(int argc, char *argv[])
{
	struct http_server_s* server = http_server_init(8080, handle_request);
	http_server_listen(server); /* non return */
}

static void handle_request(struct http_request_s *request)
{
	int status = 404;
	char body[64] = { 0 };
	int body_len = sizeof(body);

	int hwres = HW_ACTION_NOT_FOUND;
	int hw_action_nf_flag = 1;
	struct hw_action action = { 0 };

	struct http_response_s* response = http_response_init();
	struct http_string_s target = http_request_target(request);

	printf("[DBG] target=%.*s\n", target.len, target.buf);

	for (int i = 0; i < sizeof(web_apis_list)/sizeof(web_apis_list[0]); ++i) {
		if (web_apis_list[i].web_cmd == NULL) {
			break;
		}

		if (!memcmp(web_apis_list[i].web_cmd, target.buf, strlen(web_apis_list[i].web_cmd))) {
			action.command = web_apis_list[i].hw_cmd;
			hw_action_nf_flag = 0;
			break;
		}
	}

	/* don't call HW if couldn't find needed cmd, because hw operations take long time */
	if (!hw_action_nf_flag) {
		hwres = lamp_process_request(&action);
	}

	status = (hwres == HW_ACTION_SUCCESS) ? 200 : 500;
	status = (hwres == HW_ACTION_NOT_FOUND) ? 404 : status;

	/* do json */
	body_len = sprintf(body, "{\"hw-answer\": \"%s\"}",
					   hw_action_nf_flag == 0 ? action.answer : "Not found");
	if (body_len < 0) {
		status = 500;
	}

	printf("[DBG] status=%d\tbody=%s\n", status, body);

	http_response_status(response, status);
	http_response_header(response, "Content-Type", "text/plain");
	http_response_body(response, (const char *)body, body_len);
	http_respond(request, response);
}

static int lamp_process_request(struct hw_action *action)
{
	int fd, res;
	int ret = HW_ACTION_FAULURE;

	/* Serial interface to STM32 board */
	static const char port[] = "/dev/ttyUSB0";

	fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("[ERR] errno=%d opening %s: %s\n", errno, port, strerror(errno));
		return ret;
	}

	/* send command */
	printf("[TX]: %s\n", action->command);
	res = write(fd, action->command, strlen(action->command));
	if (res < 0) {
		printf("[ERR] errno=%d writting %s: %s\n", errno, port, strerror(errno));
		goto exit_err;
	}

	/* sleep some time to get answer from HW, because UART HW control is disabled */
	usleep(200);

	/* read answer */
	res = read(fd, action->answer, sizeof(action->answer));
	if (res < 0) {
		printf("[ERR] errno=%d reading %s: %s\n", errno, port, strerror(errno));
		goto exit_err;
	} else {
		printf("[RX]: %s\n", action->answer);
		ret = HW_ACTION_SUCCESS;
	}

exit_err:
	close(fd);
	return ret;
}
