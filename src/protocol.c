#include "protocol.h"
#include <stdarg.h>

int send_message(const int msgtype, ...) {
	short i = 0;
	va_list vl;
	va_start(vl, msgtype);
	if (msgtype == LOGIN || msgtype == LOGOUT) {
		Msg_login * msg = va_arg(vl, Msg_login *);
		while (msgsnd(serv_id, msg, sizeof(Msg_login) - sizeof(long), IPC_NOWAIT) == FAIL && i < MAX_FAILS) {
			++i;
			msleep(5);
		}
	}
	else if (msgtype == RESPONSE) {
		Msg_response * msg = va_arg(vl, Msg_response *);
		while (msgsnd(serv_id, msg, sizeof(Msg_response) - sizeof(long), IPC_NOWAIT) == FAIL && i < MAX_FAILS) {
			++i;
			msleep(5);
		}
	}
	else if (msgtype == REQUEST) {
		Msg_request * msg = va_arg(vl, Msg_request *);
		while (msgsnd(serv_id, msg, sizeof(Msg_request) - sizeof(long), IPC_NOWAIT) == FAIL && i < MAX_FAILS) {
			++i;
			msleep(5);
		}
	}
	else if (msgtype == MESSAGE) {
		Msg_chat_message * msg = va_arg(vl, Msg_chat_message *);
		while (msgsnd(serv_id, msg, sizeof(Msg_chat_message) - sizeof(long), IPC_NOWAIT) == FAIL && i < MAX_FAILS) {
			++i;
			msleep(5);
		}
    }	
	
	else if (msgtype == ROOM) {
		Msg_room * msg = va_arg(vl, Msg_room *);
		while (msgsnd(serv_id, msg, sizeof(Msg_room) - sizeof(long), IPC_NOWAIT) == FAIL && i < MAX_FAILS) {
			++i;
			msleep(5);
		}
	}
	else i = MAX_FAILS;

	va_end(vl);
	return (i<MAX_FAILS) ? 0 : FAIL;
}

int receive_message(const int msgtype, ...) {
	short i = 0;
	va_list vl;
	va_start(vl, msgtype);
    if (msgtype == RESPONSE) {
        Msg_response * msg = va_arg(vl, Msg_response *);
        i = msgrcv(own_id, msg, sizeof(Msg_response) - sizeof(long), msgtype, IPC_NOWAIT);
    }
	else if (msgtype == MESSAGE) {
		Msg_chat_message * msg = va_arg(vl, Msg_chat_message *);
		i = msgrcv(own_id, msg, sizeof(Msg_chat_message) - sizeof(long), msgtype, IPC_NOWAIT);
	}
	else if (msgtype == USERS || msgtype == ROOMS || msgtype == ROOM_USERS) {
		Msg_request_response * msg = va_arg(vl, Msg_request_response *);
		i = msgrcv(own_id, msg, sizeof(Msg_request_response) - sizeof(long), msgtype, IPC_NOWAIT);
	}
	else i = FAIL;
	va_end(vl);
	return i; 
}
