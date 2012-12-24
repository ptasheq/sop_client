#include "io.h"

short readstr(char * str, int n) {
	if (mvwgetnstr(stdscr, rows-1, 0, str, n) == OK) {
		clrtoeol();
		return 1;
	}
	return 0;
}

void writestr(char * str) {
	static int i = 0;
	if (str[0]) {
        mvwaddstr(chatbox, i++, 1, str);
        if (i == rows-CHATBOX_BOTTOM_SPAN - 2) {  
            wscrl(chatbox, 5);
            i -= 5;
        }
		wrefresh(chatbox);
	}
}
