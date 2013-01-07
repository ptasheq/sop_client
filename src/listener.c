#include "listener.h"
#include "communication.h"
#include "init.h"
#include "io.h"
#include "login.h"

int Pdesc[2];
int Pdesc2[2];

void listener_init() {
	if (pipe(Pdesc) != FAIL) {
		if (fcntl(Pdesc[0], F_SETFL, 0666 | O_NONBLOCK) != FAIL) /* We don't want to hang forever if sth goes wrong */
			return;
	}
	if (pipe(Pdesc2) != FAIL) {
		if (fcntl(Pdesc2[0], F_SETFL, 0666 | O_NONBLOCK) != FAIL) /* We don't want to hang forever if sth goes wrong */
			return;
	}
	perror("Couldn't create pipe.");
	end();
}

void listener_loop() {
	int fv;
	if (!(fv = fork())) {
		while (1) {
			if (is_logged()) {
				Msg_request heartbeat = {REQUEST, PONG, "\0"};
				Msg_request_response mrr_data;
				Msg_chat_message mcm_data;
				int await_message[ROOM_USERS + 1] = {0};
				int buf, i;
				char received = 1;
				strcpy(heartbeat.user_name, username);
				while (is_logged()) {
					i = 0;
					if (read(Pdesc2[0], &buf, 4) != FAIL && buf >= LOGIN && buf <= ROOM_USERS) {
						await_message[buf] = AWAIT;	
					}
					if (receive_message(RESPONSE, &response_data) != FAIL) {
						if (response_data.response_type == PING) {
							send_message(heartbeat.type, &heartbeat);
						}
						else if (await_message[RESPONSE]){
							write(Pdesc[1], &received, 1);
							write(Pdesc[1], &response_data.response_type, sizeof(int));
							write(Pdesc[1], response_data.content, RESPONSE_LENGTH);
							await_message[RESPONSE] = NOAWAIT;
							
						}
					}
					else if (receive_message(MESSAGE, &mcm_data) != FAIL) {
						print_msg(&mcm_data);
					}
					else if (await_message[USERS] && receive_message(USERS, mrr_data) != FAIL) {
						write(Pdesc[1], &received, 1);
						while (i < MAX_SERVERS_NUMBER * MAX_USERS_NUMBER && mrr_data.content[i][0]) {
							write(Pdesc[1], mrr_data.content[i], USER_NAME_MAX_LENGTH);
							i++;
						}
						await_message[USERS] = NOAWAIT;
					}
					else if (receive_message(ROOMS, mrr_data) != FAIL) {
						write(Pdesc[1], &received, 1);
						while (i < MAX_SERVERS_NUMBER * MAX_USERS_NUMBER && mrr_data.content[i][0]) {
							write(Pdesc[1], mrr_data.content[i], USER_NAME_MAX_LENGTH);
							i++;
                        }
						await_message[ROOMS] = NOAWAIT;
					}
					else if (receive_message(ROOM_USERS, mrr_data) != FAIL) {
						write(Pdesc[1], &received, 1);
						while (i < MAX_SERVERS_NUMBER * MAX_USERS_NUMBER) { 
							write(Pdesc[1], mrr_data.content[i], USER_NAME_MAX_LENGTH && mrr_data.content[i][0]);
							i++;
						}
						await_message[ROOM_USERS] = NOAWAIT;
					}
				}
			}
			msleep(100);
		}
	}
	else if (fv == FAIL) {
		perror("Couldn't create thread.");
		end();
	}
}

void listener_end() {
	close(Pdesc[1]);
	close(Pdesc[0]);
	close(Pdesc2[1]);
	close(Pdesc2[0]);
}

void print_msg(Msg_chat_message * mcm) {
	char buf[USER_NAME_MAX_LENGTH + 10];
	sprintf(buf, "[%s]: %s:", mcm->sender, mcm->send_time);
	writestr(buf);
	writestr(mcm->message);
}
