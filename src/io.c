#include "io.h"

short readstr(char * str, int n) {
	if (mvwgetnstr(stdscr, rows-1, 0, str, n) == OK) {
		clrtoeol();
		return 1;
	}
	return FAIL;
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

void readint(int * n) {
	char buf[INT_AS_STR_LENGTH];
	int i = 0, len;
	*n = 0;
	if (readstr(buf, INT_AS_STR_LENGTH) && !wants_exit(buf)) {
		writestr(buf);
		len = strlen(buf);	
		while (buf[i] && i < INT_AS_STR_LENGTH) {
			if ((buf[i] - ASCII_TO_INT) >= 0 && (buf[i] - ASCII_TO_INT <= 9)) {
				*n += power(10, --len) * (buf[i++] - ASCII_TO_INT);
			}
			else {
				*n = -1;
				break;
			}
		}
	}
	else if (wants_exit(buf)) {
		end();
	}
	else {
		*n = FAIL;
	}
}

int power(int base, int exp) {
	int i, res = base;
	if (!exp) {
		return 1;
	}
	for (i = 0; i < exp-1; ++i) {
		res *= base;
	}
	return res;
}
