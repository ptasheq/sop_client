#include "listener.h"
#include "communication.h"
#include "login.h"
#include <time.h>

int Pdesc[2];
int Pdesc2[2];

void listener_init() {
	if (pipe(Pdesc) != FAIL) {
		if (fcntl(Pdesc[0], F_SETFL, 0666 | O_NONBLOCK) != FAIL) { /* We don't want to hang forever if sth goes wrong */
			if (pipe(Pdesc2) != FAIL) {
				if (fcntl(Pdesc2[0], F_SETFL, 0666 | O_NONBLOCK) != FAIL) {
					listener_loop();
					return;
				}
			}
		}
	}
	perror("Couldn't create pipe.");
	end();
}

void listener_loop() {
	if (!(ch_pid = fork())) {
		set_signal(SIGINT, end_thread);
		set_signal(SIGPIPE, end_thread); /* in case of abnormal parent exit */
		set_signal(SIGLOG, change_login_state);
		while (1) {
			if (is_logged()) {
				Msg_request heartbeat = {REQUEST, PONG, "\0"};
				Msg_request_response mrr_data;
				Msg_chat_message mcm_data;
				int await_message[ROOM_USERS + 1] = {0};
				int buf, i;
				char received = 1;
				time_t t = time(NULL); /* for checking timeout */
				strcpy(heartbeat.user_name, username);
				while (is_logged()) {
					i = 0;
					if (read(Pdesc2[0], &buf, sizeof(int)) != FAIL && buf >= LOGIN && buf <= ROOM_USERS) {
						write(Pdesc[1], &received, sizeof(char));
						await_message[buf] = AWAIT;
					}
					if (receive_message(RESPONSE, &response_data) != FAIL) {
						if (response_data.response_type == PING) {
							t = time(NULL);
							send_message(heartbeat.type, &heartbeat);
						}
						else if (await_message[RESPONSE]) {
							write(Pdesc[1], &response_data.response_type, sizeof(int));
							write(Pdesc[1], response_data.content, RESPONSE_LENGTH);
							await_message[RESPONSE] = NOAWAIT;
						}
					}
					else if (receive_message(MESSAGE, &mcm_data) != FAIL) {
						print_msg(getppid(), &mcm_data);
					}
					else if (await_message[USERS] && receive_message(USERS, mrr_data) != FAIL) {
						while (i < MAX_SERVERS_NUMBER * MAX_USERS_NUMBER && mrr_data.content[i][0]) {
							write(Pdesc[1], mrr_data.content[i], USER_NAME_MAX_LENGTH);
							i++;
						}
						await_message[USERS] = NOAWAIT;
					}
					else if (await_message[ROOMS] && receive_message(ROOMS, mrr_data) != FAIL) {
						while (i < MAX_SERVERS_NUMBER * MAX_USERS_NUMBER && mrr_data.content[i][0]) {
							write(Pdesc[1], mrr_data.content[i], USER_NAME_MAX_LENGTH);
							i++;
                        }
						await_message[ROOMS] = NOAWAIT;
					}
					else if (await_message[ROOM_USERS] && receive_message(ROOM_USERS, mrr_data) != FAIL) {
						while (i < MAX_SERVERS_NUMBER * MAX_USERS_NUMBER && mrr_data.content[i][0]) { 
							write(Pdesc[1], mrr_data.content[i], USER_NAME_MAX_LENGTH);
							i++;
						}
						await_message[ROOM_USERS] = NOAWAIT;
					}
					if (time(NULL) - t >= TIMEOUT) { /*server is not responding */
						t = time(NULL);
						kill(getppid(), SIGTIMEOUT);
						break;
					}
					msleep(WAIT_TIME);
				}
			}
			msleep(100);
		}
	}
	else if (ch_pid == FAIL) {
		perror("Couldn't create thread.");
		end();
	}
	close(Pdesc[1]);
}

void end_thread() {
	close(Pdesc[1]);
	close(Pdesc2[0]);
	exit(EXIT_SUCCESS);
}

void listener_end() {
	close(Pdesc2[1]);
	kill(ch_pid, SIGINT);
	close(Pdesc[0]);
}

void change_login_state() {
	set_signal(SIGLOG, change_login_state);
	logged = !logged;
	if (logged) {
		int i = 0;
		char received = 1;
		while (read(Pdesc2[0], &serv_id, sizeof(int)) == FAIL && i < MAX_FAILS) {
			msleep(WAIT_TIME);
			++i;
		}
		if (i == MAX_FAILS) {
			return;
		}
		i = 0;
		while (read(Pdesc2[0], &own_id, sizeof(int)) == FAIL && i < MAX_FAILS) {
			msleep(WAIT_TIME);
			++i;
		}
		if (i == MAX_FAILS) {
			return;
		}
		/* Everything went fine */
	}
}

void print_msg(int p_pid, Msg_chat_message * mcm) {
	char buf[USER_NAME_MAX_LENGTH + 10];
	sprintf(buf, "[%s]: %s:", mcm->sender, mcm->send_time);
	kill(p_pid, SIGCHAT);
	write(Pdesc[1], buf, USER_NAME_MAX_LENGTH + 10);
	write(Pdesc[1], mcm->message, STR_BUF_SIZE);

}
