#include "listener.h"
#include "communication.h"
#include "login.h"
#include <time.h>
#include <errno.h>

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
				int await_message[ROOM_USERS + 1] = {NOAWAIT};
				int buf, i;
				FILE * fp = fopen("plik.txt", "w+");
				char received=1;
				char str[10];
				time_t t = time(NULL); /* for checking timeout */
				strcpy(heartbeat.user_name, username);
				while (is_logged()) {
					i = 0;
					if (read(Pdesc2[0], &buf, sizeof(int)) > 0 && buf >= LOGIN && buf <= ROOM_USERS) {
						write(Pdesc[1], &received, sizeof(char));
						await_message[buf] = AWAIT;
					}
					if (receive_message(RESPONSE, &response_data) != FAIL) {
						if (response_data.response_type == PING) {
							t = time(NULL);
							send_message(heartbeat.type, &heartbeat);
						}
						else if (await_message[RESPONSE]) {
							pipewrite(Pdesc[1], Pdesc2[0], &response_data.response_type, sizeof(int));
							pipewrite(Pdesc[1], Pdesc2[0], response_data.content, strlen(response_data.content));
							fprintf(fp, "msg: %d\n", response_data.response_type);
							await_message[RESPONSE] = NOAWAIT;
						}
					}
					if (receive_message(MESSAGE, &mcm_data) != FAIL) {
						pipe_msg(getppid(), &mcm_data);
					}
					if (await_message[USERS] && receive_message(USERS, &mrr_data) != FAIL) {
						while (i < MAX_SERVERS_NUMBER * MAX_USERS_NUMBER && mrr_data.content[i][0]) {
							pipewrite(Pdesc[1], Pdesc2[0], mrr_data.content[i], USER_NAME_MAX_LENGTH);
							i++;
						}
						await_message[USERS] = NOAWAIT;
					}
					else if (await_message[ROOMS] && receive_message(ROOMS, &mrr_data) != FAIL) {
						while (i < MAX_SERVERS_NUMBER * MAX_USERS_NUMBER && mrr_data.content[i][0]) {
							pipewrite(Pdesc[1], Pdesc2[0], mrr_data.content[i], ROOM_NAME_MAX_LENGTH);
							i++;
                        }
						await_message[ROOMS] = NOAWAIT;
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
	if (!logged) {
		int i = 0;
		if (piperead(Pdesc[1], Pdesc2[0], &serv_id, sizeof(int)) == FAIL)
			return;
		if (piperead(Pdesc[1], Pdesc2[0], &own_id, sizeof(int)) == FAIL)
			return;
		if (piperead(Pdesc[1], Pdesc2[0], username, USER_NAME_MAX_LENGTH) == FAIL)
			return;
		/* Everything went fine */
	}
	logged = !logged;
}

void pipe_msg(int p_pid, Msg_chat_message * mcm) {
	char buf[USER_NAME_MAX_LENGTH + 10];
	sprintf(buf, "[%s]: %s:", mcm->sender, mcm->send_time);
	if (p_pid > FAIL) {
		kill(p_pid, SIGCHAT);
		pipewrite(Pdesc[1], Pdesc2[0], (char *) &mcm->msg_type, sizeof(char));
		pipewrite(Pdesc[1], Pdesc2[0], buf, USER_NAME_MAX_LENGTH + 10);
		pipewrite(Pdesc[1], Pdesc2[0], mcm->message, STR_BUF_SIZE);
	}
}
