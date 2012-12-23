#include "libs.h"

void program_loop() {
	char str[STR_BUF_SIZE];
	getmaxyx(stdscr, rows, cols);
//	register_user_name(usrname);
	while (readstr(str, STR_BUF_SIZE) && strcmp("exit", str) != 0) {
			writestr(str);
		}	
}

