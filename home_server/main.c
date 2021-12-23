#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define HTTPSERVER_IMPL
#include "httpserver.h"

/* Lamp WEB API */
static const char lamp_on[] = "/on";
static const char lamp_off[] = "/off";
static const char err_target[] = "/error";

/* Lamp HW API */
static const char hw_lamp_on_cmd[] = "L1\n";
static const char hw_lamp_on_cmd_len = sizeof(hw_lamp_on_cmd) - 1;
static const char hw_lamp_off_cmd[] = "L0\n";
static const char hw_lamp_off_cmd_len = sizeof(hw_lamp_off_cmd) - 1;
static const char hw_ok_answer[] = "OK\n\r";
static const char hw_er_answer[] = "ER\n\r";
static const char hw_not_found[] = "NF\n\r";
static const int hw_answer_len = sizeof(hw_ok_answer) - 1; /* keep answer lengths same */

/* Error codes from STM32 */
enum {
	HW_ACTION_SUCCESS,
	HW_ACTION_FAULURE,
	HW_ACTION_NOT_FOUND,
};

static int lamp_process_request(const char *hw_action, size_t len); /* if succes Returns 'HW_ACTION_SUCCESS' */
static void handle_request(struct http_request_s *request); /* common http requests handler */

int main(int argc, char *argv[])
{
	struct http_server_s* server = http_server_init(8080, handle_request);
	http_server_listen(server); /* non return */
}

static void handle_request(struct http_request_s *request)
{
	int status = 404;
	int hwres = HW_ACTION_NOT_FOUND;
	char body[64] = { 0 };
	int body_len = sizeof(body);

	struct http_response_s* response = http_response_init();
	struct http_string_s target = http_request_target(request);

	printf("[DBG] target=%.*s\n", target.len, target.buf);

	if (!memcmp(lamp_on, target.buf, hw_lamp_on_cmd_len)) {
		printf("[DBG] lamp_on\n");
		hwres = lamp_process_request(hw_lamp_on_cmd, hw_lamp_on_cmd_len);
	}
	
	if (!memcmp(lamp_off, target.buf, hw_lamp_off_cmd_len)) {
		printf("[DBG] lamp_off\n");
		hwres = lamp_process_request(hw_lamp_off_cmd, hw_lamp_off_cmd_len);
	}

	status = (hwres == HW_ACTION_SUCCESS) ? 200 : 500;
	status = (hwres == HW_ACTION_NOT_FOUND) ? 404 : status;

	/* do json */
	body_len = sprintf(body, "{\"status\": \"%s\"}",
					   hwres == HW_ACTION_SUCCESS ? "OK" : "ERROR");
	if (body_len < 0) {
		status = 500;
	}

	printf("[DBG] status=%d\tbody=%s\n", status, body);

	http_response_status(response, status);
	http_response_header(response, "Content-Type", "text/plain");
	http_response_body(response, (const char *)body, body_len);
	http_respond(request, response);
}

static int lamp_process_request(const char *hw_action, size_t len)
{
	int fd, res;
	int ret = HW_ACTION_FAULURE;
	char rx[16] = { 0 };

	/* Serial interface to STM32 board */
	static const char port[] = "/dev/ttyUSB0";

	fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("[ERR] errno=%d opening %s: %s\n", errno, port, strerror(errno));
		return ret;
	}

	/* send command */
	res = write(fd, hw_action, len);
	if (res < 0) {
		printf("[ERR] errno=%d writting %s: %s\n", errno, port, strerror(errno));
		goto exit_err;
	}

	/* sleep some time to get answer from HW, because UART HW control is disabled */
	usleep(200);

	/* read answer */
	res = read(fd, rx, sizeof(rx));
	if (res < 0) {
		printf("[ERR] errno=%d reading %s: %s\n", errno, port, strerror(errno));
		goto exit_err;
	} else {
		printf("[RX]: %s", rx);

		if (res != hw_answer_len) {
			printf("[ERR] wrong answer len: need=%d, but got=%d", hw_answer_len, res);
			goto exit_err;
		}

		if (memcmp(rx, hw_ok_answer, hw_answer_len) != 0) {
			goto exit_err;
		}
	}

exit_ok:
	ret = HW_ACTION_SUCCESS;

exit_err:
	close(fd);
	return ret;
}
