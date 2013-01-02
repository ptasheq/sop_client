#ifndef STRUCTMEM_H
#define STRUCTMEM_H

#include "libs.h"
#include "protocol.h"

#define free_mem(ptr) {free(ptr); ptr = NULL;}

Msg_chat_message * allocate_mem(Msg_chat_message **);

#endif
