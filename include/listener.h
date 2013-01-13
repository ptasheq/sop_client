#ifndef LISTENER_H
#define LISTENER_H

#include "libs.h"
#include "protocol.h"

#define TIMEOUT 5

void listener_init(); /* has to be called before listener_loop */
void listener_loop(); /* reads messages from message queues and performs actions */
void end_thread(); /* ends thread created with fork() */
void change_login_state();
void listener_end(); /* calls thread_end and frees resources */
void print_msg(int, Msg_chat_message *); /* sends signal to parent, and conveys the message */
#endif
