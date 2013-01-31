#include "loop.h"
#include "io.h"
#include "login.h"
#include "structmem.h"
#include "communication.h"
#include "listener.h"

void program_loop() {
	char str[STR_BUF_SIZE];
	writestr("In case you need help, type \"[help]\".");
	set_signal(SIGCHAT, print_msg);
	while (readstr(str, STR_BUF_SIZE) != FAIL && !wants_exit(str)) {
		perform_action(str);	
	}
}

void perform_action(char * str) {
	if (wants_send_chat_message(str)) {
		send_chatmsg(str);
	}
	else if (wants_login(str)) {
		login();
	}
	else if (wants_logout(str)) {
		logout(0);
	}
	else if (wants_help(str)) {
		writestr("(obviously <> brackets are not required)");
		writestr("<message> -> sends public msg,  [user] <message> -> sends priv,");
		writestr("[login] -> signs in, [logout] -> signs out, [exit] -> closes program,");
		writestr("[join] <room> -> joins requested room, [leave] -> leaves room,");
		writestr("[users] -> displays users list, [rooms] -> displays rooms list,");
		writestr("[roomusers] -> displays users in the room.");
	}
	else if (wants_join_room(str)) {
		if (str[6] == ' ' && str[7]) {
			join(&str[7]);
		}
		else {
			writestr("Room name must contain at least one character.");
		}
	}
	else if (wants_leave_room(str)) {
		leave();
	}
	else if (wants_users_list(str)) {
		request(USERS_LIST);	
	}
	else if (wants_rooms_list(str)) {
		request(ROOMS_LIST);
	}
	else if (wants_room_users_list(str)) {
		writestr("Option has not been implemented.");
		/*request(ROOM_USERS_LIST);*/
	}
	else { /* private message or some trash*/
		if (str[0] == '[') {
			short i = 1;
			while (str[i]) {
				if (str[i] == ']' && i > 1) { /* there is something between brackets */
					i = 0;
					break;
				}
				++i;
			}
			if (!i) {
				send_priv(str);
			}
			else {
				writestr("Unknown option.");
			}
		}
	}
	
}
