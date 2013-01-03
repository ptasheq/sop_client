#include "loop.h"
#include "io.h"
#include "login.h"
#include "structmem.h"
#include "communication.h"

void program_loop() {
	char str[STR_BUF_SIZE];
	writestr("In case you need help, type \"[help]\".");
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
		logout();
	}
	else if (wants_help(str)) {
		writestr("(obviously <> brackets are not required)");
		writestr("<message> -> sends public msg,  [user] <message> -> sends priv,");
		writestr("[login] -> signs in, [logout] -> signs out, [exit] -> closes program.");
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
	else { /* private message */
		send_priv(str);
		writestr("priv");
		
	}
	
}

