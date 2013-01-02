#include "structmem.h"

Msg_chat_message * allocate_mem(Msg_chat_message ** ptr) {
	if (!(*ptr)) {
		return (*ptr = malloc(sizeof(Msg_chat_message)));
	}
	else return *ptr; /* there is no need for allocation */
}
