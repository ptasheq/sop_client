#include "structmem.h"

void * allocate_mem(const int type, ...) {
	va_list vl;
	va_start(vl, type);
	if (type == LOGIN || type == LOGOUT) {
		Msg_login ** msg = va_arg(vl, Msg_login **);
		if (!(*msg))
			return (*msg = malloc(sizeof(Msg_login)));
	}
	else if (type == ROOM) {
		Msg_room ** msg = va_arg(vl, Msg_room **);
		if (!(*msg))
			return (*msg = malloc(sizeof(Msg_room)));
	}
	else if (type == REQUEST) {
		Msg_request ** msg = va_arg(vl, Msg_request **);
		if (!(*msg))
			return (*msg = malloc(sizeof(Msg_request)));
	}
    else if (type == USERS || type == ROOMS || type == ROOM_USERS) {
        Msg_request_response ** msg = va_arg(vl, Msg_users_list **);
        if (!(*msg))
            return (*msg = malloc(sizeof(Msg_users_list)));
    }
	return (void *)1; /* there is no need for allocation */
}
