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
	key_t own_num;
	struct msqid_ds stats;
	set_signal(SIGTIMEOUT, logout);
	while (!logged) {
		writestr("Please insert server queue id:");
		readint(&serv_id);
		while (serv_id == FAIL || msgctl(serv_id, IPC_STAT, &stats) == FAIL) {
			writestr("Please insert correct queue id:");
			readint(&serv_id);
		}
		
		writestr("Please choose client address(queue number):");
		readint(&own_num);
		while (own_num == FAIL || (own_id = msgget(own_num, IPC_CREAT | 0666)) == FAIL || own_id == serv_id) {
			writestr("Please enter correct address(different from server):");
			readint(&own_num);
		}
		if (allocate_mem(LOGIN, &login_data)) {
			writestr("Please enter your nickname:");
			if (readstr(login_data->username, USER_NAME_MAX_LENGTH) != FAIL && !wants_exit(login_data->username)) {
				login_data->ipc_num = own_id;
				login_data->type = LOGIN;
				if (send_message(login_data->type, login_data) != FAIL) {
					msleep(WAIT_TIME);
					if (receive_message(RESPONSE, &response_data) != FAIL &&
						response_data.response_type == LOGIN_SUCCESS) {
						writestr(response_data.content);
						logged = 1;
						kill(ch_pid, SIGLOG);
						pipewrite(Pdesc2[1], Pdesc[0], &serv_id, sizeof(int));
						pipewrite(Pdesc2[1], Pdesc[0], &own_id, sizeof(int));
						pipewrite(Pdesc2[1], Pdesc[0], login_data->username, strlen(login_data->username));
						strcpy(username, login_data->username);
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
			login_data->ipc_num = own_id;
			strcpy(login_data->username, username);
			if (send_message(login_data->type, login_data) != FAIL) {
				if (wait_until_received(RESPONSE) > FAIL) {
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
	inroom = 0;
	set_signal(SIGTIMEOUT, logout);
}
