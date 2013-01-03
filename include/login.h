#ifndef LOGIN_H
#define LOGIN_H

#include "libs.h"
#include "protocol.h"

#define is_logged() logged

extern short logged;
extern char username[USER_NAME_MAX_LENGTH];

void login(); /* connects with server, gets username */
void logout(); /* disconnects from server */

#endif
