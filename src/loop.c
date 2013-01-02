#include "loop.h"
#include "io.h"
#include "login.h"
#include "structmem.h"
#include <time.h>

Msg_chat_message * chatmsg_data = NULL;

void program_loop() {
	char str[STR_BUF_SIZE];
	writestr("In case you need help, type \"[help]\".");
	while (readstr(str, STR_BUF_SIZE-1) != FAIL && !wants_exit(str)) {
		perform_action(str);	
	}	
}

void perform_action(char * str) {
	if (is_logged()) {
		if (wants_send_chat_message(str)) {
			writestr("sendmsg");	
		}
		else if (wants_login(str)) {
			logout();
			login();
		}
		else if (wants_logout(str)) {
			writestr("logout");
			logout();
		}
		else if (wants_help(str)) {
			writestr("(obviously <> brackets are not required)");
			writestr("<message> -> sends public msg,  [user] <message> -> sends priv,");
			writestr("[login] -> signs in, [logout] -> signs out, [exit] -> closes program.");
		}
		else { /* private message */
			writestr("priv");
			int username_length;
			if ((username_length = get_username(str)) != FAIL) {
				if (allocate_mem(&chatmsg_data)) {
					strcpy(chatmsg_data->receiver, str);
					strcpy(chatmsg_data->message, &str[username_length+1]);
					if (chatmsg_data->message[0]) {
						get_time(chatmsg_data->send_time);
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
					writestr("Couldn't allocate chatmsg_data structure");
				}
			}
			else {
				writestr("This priv doesn't make sense.");
			}
		}
	}
	else if (wants_login(str)) {
		login();
	}
	else if (wants_help(str)) {
		writestr("(obviously <> brackets are not required)");
        writestr("<message> -> sends public msg,  [user] <message> -> sends priv,");
        writestr("[login] -> signs in, [logout] -> signs out, [exit] -> closes program.");
	}
	else {
		writestr("To perform this action, you have to be logged in.");
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

