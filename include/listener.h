#ifndef LISTENER_H
#define LISTENER_H

#include "libs.h"
#include "protocol.h"

void listener_init(); /* has to be called before listener_loop */
void listener_loop(); /* reads messages from message queues and performs actions */
void listener_end();
void print_msg(Msg_chat_message *);
#endif
