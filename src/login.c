#include "login.h"
#include "io.h"

Msg_login login_data;
Msg_response response_data;
key_t serv_num;
int serv_id, own_id;
short logged = 0;
char username[USER_NAME_MAX_LENGTH];

void login() {
	key_t own_num;
	while (!logged) {
		writestr("Please insert server address(queue number):");
		readint(&serv_num);
		while ((serv_id = msgget(serv_num, 0666)) == FAIL || serv_num == FAIL) {
			writestr("Please insert correct address:");
			readint(&serv_num);
		}
		writestr("Please choose client address(queue number):");
		readint(&own_num);
		while (own_num == FAIL || own_num == serv_num || (own_id = msgget(own_num, IPC_CREAT | 0666)) == FAIL) {
			writestr("Please enter correct address(different from server):");
			readint(&own_num);
		}
		writestr("Please enter your nickname:");
		if (readstr(login_data.username, USER_NAME_MAX_LENGTH) != FAIL && !wants_exit(login_data.username)) {
			login_data.ipc_num = own_num;
			login_data.type = LOGIN;
			if (send_message(login_data.type, &login_data) != FAIL) {
				if (receive_message(RESPONSE, &response_data) != FAIL &&
					response_data.response_type == LOGIN_SUCCESS) {
					writestr(response_data.content);
					logged = 1;
					strcpy(username, login_data.username);
				}
				else if (response_data.response_type == LOGIN_FAILED) {
					writestr("Server rejected connection");
				}
				else {
					writestr("Login failed");
				}
			}
			
		}
		else if (wants_exit(login_data.username)) {
			end();
		}

	}
}

void logout() {
	if (is_logged()) {
		logged = 0;
		login_data.type = LOGOUT;
		strcpy(login_data.username, username);
		if (send_message(login_data.type, &login_data) != FAIL && receive_message(RESPONSE, &response_data) != FAIL) {
			if (response_data.response_type == LOGOUT_SUCCESS) {
				writestr("Successfully logout.");
			}
			else {
				writestr("Must have lost connection before logout.");
			}
		}
		else {
			writestr("Couldn't contact server, so assuming logout");
		}
	}
}


