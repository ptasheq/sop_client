#include "login.h"

Msg_login login_data;

void login() {
	char buf [STR_BUF_SIZE];
	writestr("Proszę podać nazwę użytownika:");
	readstr(login_data.username, USER_NAME_MAX_LENGTH-1);
	login_data.type = LOGIN;
	strcpy(buf, "Witaj ");
	strcat(buf, login_data.username);
	writestr(buf);
}

void logout() {

}
