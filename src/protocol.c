#include "protocol.h"
#include <stdarg.h>

int send_message(int msgtype, ...) {
	short i = 0;
	va_list vl;
	va_start(vl, msgtype);
	if (msgtype == LOGIN || msgtype == LOGOUT) {
		Msg_login * msg = va_arg(vl, Msg_login *);
		while (msgsnd(serv_id, msg, USER_NAME_MAX_LENGTH + sizeof(key_t), IPC_NOWAIT) == FAIL && i < MAX_FAILS) {
			++i;
			msleep(20);
		}
	}
	else if (msgtype == RESPONSE) {
		Msg_response * msg = va_arg(vl, Msg_response *);
		while (msgsnd(serv_id, msg, sizeof(int) + RESPONSE_LENGTH, IPC_NOWAIT) == FAIL && i < MAX_FAILS) {
			++i;
			msleep(20);
		}
	}
	else if (msgtype == REQUEST) {
		Msg_request * msg = va_arg(vl, Msg_request *);
		while (msgsnd(serv_id, msg, sizeof(int) + USER_NAME_MAX_LENGTH, IPC_NOWAIT) == FAIL && i < MAX_FAILS) {
            ++i;
            msleep(20);
		}
	}
	else if (msgtype == MESSAGE) {
		Msg_chat_message * msg = va_arg(vl, Msg_chat_message *);
		while (msgsnd(serv_id, msg, sizeof(int) + 6  + STR_BUF_SIZE + 2*USER_NAME_MAX_LENGTH, IPC_NOWAIT) == FAIL && i < MAX_FAILS) {
            ++i;
            msleep(20);
		}
	}
	va_end(vl);
	return (i<MAX_FAILS) ? 1 : FAIL;
}

int receive_message(int msgtype, ...) {
	short i = 0;
	va_list vl;
	va_start(vl, msgtype);
	if (msgtype == LOGIN || msgtype == LOGOUT) {
        Msg_login * msg = va_arg(vl, Msg_login *);
        while (msgrcv(own_id, msg, USER_NAME_MAX_LENGTH + sizeof(key_t), msgtype, IPC_NOWAIT) == FAIL && i < MAX_FAILS) {
            ++i;
            msleep(20);
        }
    }
    else if (msgtype == RESPONSE) {
        Msg_response * msg = va_arg(vl, Msg_response *);
        while (msgrcv(own_id, msg, sizeof(int) + RESPONSE_LENGTH, msgtype, IPC_NOWAIT) == FAIL && i < MAX_FAILS) {
            ++i;
            msleep(20);
        }
    }
    else if (msgtype == REQUEST) {
        Msg_request * msg = va_arg(vl, Msg_request *);
        while (msgrcv(own_id, msg, sizeof(int) + USER_NAME_MAX_LENGTH, msgtype, IPC_NOWAIT) == FAIL && i < MAX_FAILS) {
            ++i;
            msleep(20);
        }
    }
    else if (msgtype == MESSAGE) {
        Msg_chat_message * msg = va_arg(vl, Msg_chat_message *);
        while (msgrcv(own_id, msg, sizeof(int) + 6  + STR_BUF_SIZE + 2*USER_NAME_MAX_LENGTH, msgtype, IPC_NOWAIT) == FAIL && i < MAX_FAILS) {
            ++i;
            msleep(20);
        }
    }
	va_end(vl);
	return (i<MAX_FAILS) ? 1 : FAIL;
}