#include "loop.h"
#include "io.h"

void program_loop() {
	char str[STR_BUF_SIZE];
	getmaxyx(stdscr, rows, cols);
	while (readstr(str, STR_BUF_SIZE) != FAIL && !wants_exit(str)) {
			writestr(str);
		}	
}

