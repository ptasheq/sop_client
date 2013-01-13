#include "communication.h"
#include "login.h"
#include <time.h>
#include "structmem.h"
#include "io.h"
#include <signal.h>

Msg_room * room_data = NULL;
Msg_request_response * request_response_data = NULL;
Msg_request * request_data = NULL;
Msg_chat_message chatmsg_data;
char roomname[ROOM_NAME_MAX_LENGTH];
int inroom = 0;

void join(char * str) {
	if (!is_inroom() && is_logged()) {
		if (allocate_mem(ROOM, &room_data)) {
			room_data->type = ROOM;
			room_data->operation_type = ENTER_ROOM;
			strcpy(room_data->user_name, username);
			strcpy(room_data->room_name, str);
			if (send_message(room_data->type, room_data) != FAIL && wait_until_received(room_data->type) != FAIL) {
				if (response_data.response_type == ENTERED_ROOM_SUCCESS) {
					inroom = 1;
					strcpy(roomname, room_data->room_name);
				}
				else {
					writestr("Couldn't enter room.");
				}
			}
			else {
				writestr("Server didn't answer.");	
			}
			free_mem(room_data);
		}
	}
	else if (is_inroom() && is_logged()) {
		if (!strcmp(str, roomname)) { /* User didn't choose the same room to log in again */
			if (allocate_mem(ROOM, &room_data)) {
				room_data->type = ROOM;
				room_data->operation_type = CHANGE_ROOM;
				strcpy(room_data->user_name, username);
		   		strcpy(room_data->room_name, str);
				if (send_message(room_data->type, room_data) != FAIL && wait_until_received(room_data->type) != FAIL) {
           			if (response_data.response_type == CHANGE_ROOM_SUCCESS) {
						strcpy(roomname, room_data->room_name);
            		}
            		else {
                		writestr("Couldn't change room.");
            		}
        		}
				else {
					writestr("Server didn't answer");
				}
				free_mem(room_data);
			}
			else {
				writestr("Couldn't allocate room_data structure.");
			}
		}
		else {
			writestr("No use in entering the same room again.");
		}
	}
	else {
		writestr("To perform it, you have to be logged in.");
	}
}

void leave() {
	if (is_inroom()) {
		if (allocate_mem(ROOM, &room_data)) {
			room_data->operation_type = LEAVE_ROOM;
			strcpy(room_data->user_name, username);
			strcpy(room_data->room_name, roomname);
			if (send_message(room_data->type, room_data) != FAIL && wait_until_received(room_data->type) != FAIL) {
				inroom = 0;
           		if (response_data.response_type == LEAVE_ROOM_SUCCESS) {
					writestr(response_data.content);
           		}
				else {
					writestr("Some error occurred.");
				}
			}
			free_mem(room_data);
		}
		else {
			writestr("Couldn't allocate room_data structure.");
		}
	}
	else {
		writestr("To perform it, you have to be in room.");
	}
}

void send_chatmsg(char * str) {
	if (is_inroom() && is_logged()) {
		chatmsg_data.msg_type = PUBLIC;
		get_time(chatmsg_data.send_time);
		strcpy(chatmsg_data.sender, username);
		strcpy(chatmsg_data.receiver, roomname);
		strcpy(chatmsg_data.message, str);
		if (send_message(chatmsg_data.type, chatmsg_data) != FAIL && wait_until_received(chatmsg_data.type) != FAIL) {
			if (response_data.response_type == MSG_SEND) {
				writestr("message sent successfully.");
			}
			else {
				writestr("message wasn't sent.");
			}
		}
	}
	else {
		writestr("To perform it, you have to be logged in and be in room.");
	}
}

void send_priv(char * str) {
	if (is_logged()) {
		int username_length;	
		if ((username_length = get_username(str)) != FAIL) {
   	       	strcpy(chatmsg_data.receiver, str);
			strcpy(chatmsg_data.sender, username);
			strcpy(chatmsg_data.message, &str[username_length+1]);
           	if (chatmsg_data.message[0]) {
           		get_time(chatmsg_data.send_time);
				chatmsg_data.msg_type = PRIVATE;
				if (send_message(chatmsg_data.type, &chatmsg_data) != FAIL && wait_until_received(chatmsg_data.type) != FAIL) {
					if (response_data.response_type == MSG_SEND) {
						writestr("message sent successfully.");
					}
					else {
						writestr("message wasn't sent");
					}
				}
           	}
           	else {
          		writestr("No use in sending empty message.");
        	}
        }
    	else {
       		writestr("This priv doesn't make sense.");
    	}
	}
	else {
		writestr("To perform this, you have to be logged in.");
	}
}

void request(const int rtype) {
	if (is_logged() && rtype >= USERS_LIST && rtype <= ROOM_USERS_LIST ) {
		if (allocate_mem(REQUEST, &request_data)) {
			request_data->type = REQUEST;
			request_data->request_type = rtype;
			strcpy(request_data->user_name, username);
			if (send_message(request_data->type, request_data) != FAIL)  {
				int mtype = (rtype == USERS_LIST) ? USERS : (rtype == ROOMS_LIST) ? ROOMS : ROOM_USERS;
				if (allocate_mem(mtype, &request_response_data)) {
					if (wait_until_received(mtype) != FAIL) {
						display_request_result();	
					}
					free_mem(request_response_data);
				}
				else {
					writestr("Couldn't allocate data structure.");
				}
			}
			free_mem(request_data);
		}
		else {
			writestr("Couldn't allocate request_data structure.");
		}
	}
	else {
		writestr("To perform this, you have to be logged in.");
	}
}

int get_username(char * str) {
    int i = 1;
    char username[USER_NAME_MAX_LENGTH];
    while (i < USER_NAME_MAX_LENGTH && str[i] && str[i] != ']') {
        username[i-1] = str[i];
        ++i;
    }
    username[i-1] = '\0';
    if (str[i] == ']' && str[i+1] == ' ' && i > 1) { /* after ] there should be space */
        i = 0;
        while (username[i]) {
            str[i] = username[i];
            ++i;
        }
        str[i] = '\0'; /* we put null in the place of ]*/
        return i + 2; /* we return pos of space */
    }
    else return FAIL;
}

void get_time(char * str) { /* all numbers are set because of result of ctime (char *) */
    time_t curtime;
    char buf[30];
    int i = 0, spaces = 0;

    if (time(&curtime) != FAIL) {
        strncpy(buf, ctime(&curtime), 29);
        while (buf[i] && spaces < 4) {
            if (buf[i] == ' ') {
                ++spaces;
            }
            ++i;
        }
        if (spaces == 4) {
            strncpy(str, &buf[i], 5);
            str[5] = '\0';
            return;
        }
    }
    strcpy(str, "00:00");
}

void display_request_result() { 
	int i = 0;
	while (i < MAX_SERVERS_NUMBER * MAX_USERS_NUMBER && request_response_data->content[i][0]) {
		writestr(request_response_data->content[i]);
		++i;
	}
}

int wait_until_received(const int mtype) {
    if (mtype > 0 && mtype <= MSG_TYPES_NUMBER) {
        int i = 0;
        char received;
		if (write(Pdesc2[1], &mtype, sizeof(int)) != FAIL)
		msleep(10);
        while (read(Pdesc[0], &received, sizeof(char)) == FAIL && i < MAX_FAILS) {
            ++i;
            msleep(1000);
        }
        if (i == MAX_FAILS || !received)
            return FAIL;
		i = 0;
        if (mtype == RESPONSE) {
			while (read(Pdesc[0], &response_data.response_type, sizeof(int)) == FAIL && i < MAX_FAILS) {
				++i;
				msleep(10);
			} 
			while (read(Pdesc[0], response_data.content, RESPONSE_LENGTH) == FAIL && i < MAX_FAILS) {
				++i;
				msleep(10);
			}
			if (i == MAX_FAILS)
				return FAIL;
        }
		else if (mtype == USERS || mtype == ROOMS || mtype == ROOM_USERS_LIST) {
			int j = 0;
			while (read(Pdesc[0], request_response_data->content[j], USER_NAME_MAX_LENGTH) == FAIL) {
				++i;
				msleep(10);
			}
			if (i == MAX_FAILS)
				return FAIL;
			++j;
			while (read(Pdesc[0], request_response_data->content[j], USER_NAME_MAX_LENGTH) > 0 &&
				j < MAX_SERVERS_NUMBER * MAX_USERS_NUMBER && request_response_data->content[j][0]) {
				++j;
			}
		}
        if (i != MAX_FAILS)
            return 1;
    }
    return FAIL;
}

void set_signal(int signum, sa_handler handler) {
	struct sigaction sh;
	sh.sa_handler = handler;
	sigemptyset(&sh.sa_mask);
	sh.sa_flags = 0;
	sigaction(signum, &sh, NULL);
}
