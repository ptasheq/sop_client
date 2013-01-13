#include "io.h"

unsigned short int lnum = 0;
char ** lines = NULL; 
unsigned short int signal_handled = 0;

short readstr(char * str, int n) { /* Reads always one character less, so prevents overflow */
	int flag = KEY_RESIZE;
	str[0] = '\0';
	while (flag == KEY_RESIZE) {
		if ((flag = mvwgetnstr(stdscr, rows-2, 0, str, n)) == OK) {
			if (!str[0]) {
				flag = KEY_RESIZE;
				continue;
			}
			move(rows-2, 0);
			clrtoeol();
		}
		else if (flag == KEY_RESIZE) {
			readstr(str, n); /* For some reason after signal there is always one fail readstr, so we intercept it */
		}
		else if (signal_handled) {
			--signal_handled;
			flag = readstr(str, n);
		}
	}
	return flag;
}
void writestr(char * str) {
	int i = (lnum < rows) ? lnum : rows - lnum % 5;
	if (str[0]) {
        mvwaddstr(chatbox, i, 1, str);
		add_line(str);
        if (i == rows-CHATBOX_BOTTOM_SPAN - 2) {  
            wscrl(chatbox, 5);
        }
		wrefresh(chatbox);
	}
}

void readint(int * n) {
	char buf[INT_AS_STR_LENGTH];
	int i = 0, len;
	*n = 0;
	
	if (readstr(buf, INT_AS_STR_LENGTH) != FAIL && !wants_exit(buf)) {
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

void add_line(char * str) {
	int i = (lnum < MAX_LINES) ? lnum : lnum % 65000;
	if (!lines) {
		if (!(lines = malloc(LINES_MEM_SEG * sizeof(char *)))) {
			perror("Couldn't allocate memory for chat buffer.");
			end();
		}
	}
	if (lnum % LINES_MEM_SEG == 0 && lnum < MAX_LINES) {
		char ** tmp = lines;
		if (!(tmp = realloc(lines, (lnum + LINES_MEM_SEG) * sizeof(char *)))) {
			perror("Couldn't expand memory for chat buffer.");
			free(lines);
			end();
		}
		lines = tmp;
	}

	if (!(lines[i] = malloc((strlen(str) + 1) * sizeof(char)))) {
		int j;
		perror("Couldn't allocate memory for saving the line.");
		for (j = 0; j < lnum; ++j) {
			free(lines[j]);
		}
		free(lines);
		end();
	}
	/* Everything has gone fine */
	strcpy(lines[i], str);
	++lnum;
		
}

void display_lines() {
	short j = 0;
	short chatbox_rows = rows - CHATBOX_BOTTOM_SPAN - 2;
	int i = (lnum > chatbox_rows) ? lnum - chatbox_rows : 0;
	while (i < lnum) {
		mvwaddstr(chatbox, j++, 1, lines[i]); 
		++i;
	}
	wrefresh(chatbox);
}
