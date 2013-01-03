#include "communication.h"
#include "login.h"
#include <time.h>
#include "structmem.h"

Msg_room room_data = {ROOM, 0, "", ""};
int inroom = 0;
char roomname[ROOM_NAME_MAX_LENGTH];
Msg_chat_message * chatmsg_data = NULL;

void join(char * str) {
	if (!is_inroom() && is_logged()) {
		room_data.operation_type = ENTER_ROOM;
		strcpy(room_data.user_name, username);
		strcpy(room_data.room_name, str);
		if (send_message(room_data.type, &room_data) != FAIL && receive_message(RESPONSE, &response_data) != FAIL) {
			if (response_data.response_type == ENTERED_ROOM_SUCCESS) {
				inroom = 1;
				strcpy(roomname, room_data.room_name);
			}
			else {
				writestr("Couldn't enter room.");
			}
		}
		else {
			writestr("Server didn't answer.");	
		}
	}
	else if (is_inroom() && !is_logged()) {
		if (!strcmp(str, roomname)) { /* User didn't choose the same room to log in again */
			room_data.operation_type = CHANGE_ROOM;
			strcpy(room_data.user_name, username);
		    strcpy(room_data.room_name, str);
			if (send_message(room_data.type, &room_data) != FAIL && receive_message(RESPONSE, &response_data) != FAIL) {
            	if (response_data.response_type == CHANGE_ROOM_SUCCESS) {
					strcpy(roomname, room_data.room_name);
            	}
            	else {
                	writestr("Couldn't change room.");
            	}
        	}
			else {
				writestr("Server didn't answer");
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
		room_data.operation_type = LEAVE_ROOM;
		strcpy(room_data.user_name, username);
		strcpy(room_data.room_name, roomname);
		if (send_message(room_data.type, &room_data) != FAIL && receive_message(RESPONSE, &response_data) != FAIL) {
			inroom = 0;
            if (response_data.response_type == LEAVE_ROOM_SUCCESS) {
				writestr(response_data.content);
            }
			else {
				writestr("Some error occurred.");
			}
		}	
	}
	else {
		writestr("To perform it, you have to be in room.");
	}
}

void send_chatmsg(char * str) {
	if (is_inroom() && is_logged()) {
		if (allocate_mem(&chatmsg_data)) {
			chatmsg_data->type = MESSAGE;
			chatmsg_data->msg_type = PUBLIC;
			get_time(chatmsg_data->send_time);
			strcpy(chatmsg_data->sender, username);
			strcpy(chatmsg_data->receiver, roomname);
			strcpy(chatmsg_data->message, str);
			if (send_message(chatmsg_data->type, chatmsg_data) != FAIL && receive_message(RESPONSE, &response_data) != FAIL) {
				if (response_data.response_type == MSG_SEND) {
					writestr("message send successfully.");
				}
				else {
					writestr("message wasn't sent.");
				}
			}

		}
		else {
			writestr("Couldn't allocate chatmsg_data structure.");
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
   	     	if (allocate_mem(&chatmsg_data)) {
   	        	strcpy(chatmsg_data->receiver, str);
				strcpy(chatmsg_data->sender, username);
    	        strcpy(chatmsg_data->message, &str[username_length+1]);
        	    if (chatmsg_data->message[0]) {
            	    get_time(chatmsg_data->send_time);
	                chatmsg_data->type = MESSAGE;
					chatmsg_data->msg_type = PRIVATE;
   		            writestr(chatmsg_data->receiver);
                	writestr(chatmsg_data->send_time);
                	writestr(chatmsg_data->message);
           		}
           		else {
                	writestr("No use in sending empty message.");
           		}
           		free_mem(chatmsg_data);
        	}
        	else {
            	writestr("Couldn't allocate chatmsg_data structure.");
        	}
    	}
    	else {
        	writestr("This priv doesn't make sense.");
    	}
	}
	else {
		writestr("To perform it, you have to be logged in.");
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

