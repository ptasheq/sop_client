#include "login.h"
#include "io.h"
#include "structmem.h"
#include "communication.h"

Msg_login * login_data = NULL;
Msg_response response_data;
int await_message[MSG_TYPES_NUMBER] = {0};
int serv_id, own_id;
short logged = 0;
int ch_pid;
char username[USER_NAME_MAX_LENGTH];

void login() {
	key_t own_num, serv_num;
	char received;
	set_signal(SIGTIMEOUT, logout);
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
		if (allocate_mem(LOGIN, &login_data)) {
			writestr("Please enter your nickname:");
			if (readstr(login_data->username, USER_NAME_MAX_LENGTH) != FAIL && !wants_exit(login_data->username)) {
				login_data->ipc_num = own_num;
				login_data->type = LOGIN;
				if (send_message(login_data->type, login_data) != FAIL) {
					msleep(WAIT_TIME);
					if (receive_message(RESPONSE, &response_data) != FAIL &&
						response_data.response_type == LOGIN_SUCCESS) {
						writestr(response_data.content);
						logged = 1;
						kill(ch_pid, SIGLOG);
						write(Pdesc2[1], &serv_id, sizeof(int));
						write(Pdesc2[1], &own_id, sizeof(int));
						msleep(WAIT_TIME);
						if (read(Pdesc[0], &received, sizeof(char))) {
							strcpy(username, login_data->username);
						}
						else {
							strcpy(login_data->username, "[exit]");
						}
					}
					else if (response_data.response_type == LOGIN_FAILED) {
						writestr("Server rejected connection");
					}
					else {
						writestr("Login failed");
					}
				}
				free_mem(login_data);	
			}
			else if (wants_exit(login_data->username)) {
				free_mem(login_data);
				end();
			}
		}
		else {
			writestr("Couldn't allocate login_data structure.");
		}
	}
}

void logout(int flag) {
	set_signal(SIGALRM, logout);
	if (flag) { /* Serves as signal handle */
		if (!signal_handled)
			signal_handled++; 
		logged = 0;
		kill(ch_pid, SIGLOG);
		writestr("Connection lost. Logout.");
	}
	else if (is_logged()) {
		if (allocate_mem(LOGOUT, &login_data)) {
			login_data->type = LOGOUT;
			strcpy(login_data->username, username);
			if (send_message(login_data->type, login_data) != FAIL) {
				if (wait_until_received(RESPONSE)) {
					if (response_data.response_type == LOGOUT_SUCCESS) {
						writestr("Successfully logout.");
					}
					else {
						writestr("Must have lost connection before logout.");
					}
				}
				else {
					writestr("Connection lost. Logout.");
				}
			}
			else {
				writestr("Couldn't contact server, so assuming logout");
			}
			logged = 0;
			kill(ch_pid, SIGLOG);
			free_mem(login_data);
			msgctl(own_id, IPC_RMID, NULL);
		}
		else {
			writestr("Couldn't allocate login_data structure.");
		}
	}
	else {
		writestr("To perform it, you have to be logged in.");
	}
	set_signal(SIGTIMEOUT, logout);
}



