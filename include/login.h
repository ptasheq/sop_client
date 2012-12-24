#ifndef LOGIN_H
#define LOGIN_H

#include "libs.h"
#include "protocol.h"

extern Msg_login login_data;

void login(); // connects with server, gets username
void logout(); // disconnects from server

#endif
