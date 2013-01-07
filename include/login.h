#ifndef LOGIN_H
#define LOGIN_H

#include "libs.h"
#include "protocol.h"

#define is_logged() logged

extern Msg_login * login_data;
extern Msg_response response_data;

extern short logged;
extern char username[USER_NAME_MAX_LENGTH];

void login(); /* connects with server, gets username */
void logout(); /* disconnects from server */

#endif
