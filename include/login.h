#ifndef LOGIN_H
#define LOGIN_H

#include "libs.h"
#include "protocol.h"
#include <signal.h>

#define is_logged() logged
#define SIGLOG SIGALRM
#define SIGCHAT SIGALRM
#define SIGTIMEOUT SIGUSR1


extern Msg_login * login_data;
extern Msg_response response_data;

extern short logged;
extern char username[USER_NAME_MAX_LENGTH];
extern int ch_pid;

void login(); /* connects with server, gets username */
void logout(int); /* disconnects from server */

#endif
