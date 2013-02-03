#include "io.h"

Lines * lines = NULL; 
unsigned short signal_handled = 0;
unsigned short lnum = 0;
unsigned short resize = 0;
unsigned short scroll_val = 0;
unsigned short span_val;
int posy = 0;

/*short readstr(char * str, int n) {
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
			readstr(str, n); 
		}
		else if (signal_handled) {
			signal_handled--;	
			writestr(str);
			flag = readstr(str, n);
		}
	}
	return flag;
}*/

short readstr(char * str, int n) {
	short max = 0, pos = 0, pos_in_str = 0, i, polish_chars = 0;
	char letter[3]={};
	char buf;
	str[0] = '\0';
	while ((buf = mvwgetch(stdscr, rows-2, pos)) != '\n' || !str[0]) {
		if (buf == '\n')
			continue;
		else if (buf == 27) {
			if ((buf = mvwgetch(stdscr, rows-2, pos)) == 91) { /* arrow pressed */
				switch((buf = mvwgetch(stdscr, rows-2, pos))) {
					case 65: /* up */
						if (lnum-scroll_val > rows-CHATBOX_BOTTOM_SPAN-2) {
							wclear(chatbox);
							scroll_val++;
							display_lines();
						}
						break;
					case 66: /* down */
						if (scroll_val) {
							--scroll_val;
							wclear(chatbox);
							display_lines();
						}
						break;
					case 67: /* right */
						pos = (pos < max) ? pos+1 : pos;
						break;
					case 68:
						pos = (pos > 0) ? pos-1 : pos;
					default: break;
				}
				move(rows-2, pos);
			}
		}
		else if (buf == 127)  { /* backspace */
			if (pos > 0) {
				if (pos == max) {
					--pos;
					max = (str[pos] < 0) ? max-2 : max-1;
					move(rows-2, pos);
					delch();
				}
				else {
					for (i = pos; i < max; ++i)
						str[i-1] = str[i];
					--pos;
					max = (str[pos] < 0) ? max-2 : max-1;
					move(rows-2, pos);
					delch();
				}
			}
		}
		else if (is_polish(buf)) {
			letter[0] = buf;
			letter[1] = mvwgetch(stdscr, rows-2, pos);
			if (max < n-3) {
				pos_in_str = (pos < max-polish_chars) ? add_char_in_text(1, str, max, pos, polish_chars) : max;
				mvwaddstr(stdscr, rows-2, pos, letter);
				str[pos_in_str] = letter[0];
				str[pos_in_str+1] = letter[1];
				max+=2;
				polish_chars++;
				pos++;
			}
		}
		else if (buf != FAIL && buf != -102) {
			if (max < n-1) {
				pos_in_str = (pos < max-polish_chars) ? add_char_in_text(0, str, max, pos, polish_chars) : max;
				mvwaddch(stdscr, rows-2, pos, buf);
				str[pos_in_str] = buf;
				++pos;
				++max;
			}
		}
		else if (buf == -102) {
			char buf[50];
			mvwgetnstr(stdscr, rows-2, pos, buf, 50);
		}
	}
	move(rows-2, 0);
	clrtoeol();
	str[max] = '\0';
	return 0;
}

void writestr(char * str) {	
	scroll_val = 0;
	if (str[0]) {
        posy = (posy > rows-CHATBOX_BOTTOM_SPAN-2) ? rows-CHATBOX_BOTTOM_SPAN-2 : posy;
		if (posy == rows-CHATBOX_BOTTOM_SPAN - 2) {  
            wscrl(chatbox, 5);
			posy -=5;
        }
        mvwaddstr(chatbox, posy, 1, str);
		posy++; 
		span_val = rows-CHATBOX_BOTTOM_SPAN-2-posy;
		add_line(str);
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

short add_char_in_text(short flag, char * str, short max, short pos, short polish_chars) {
	short i = 0, j = 0, ret_val;
	char bufpch[3] = {};
	move(rows-2, 0);
	clrtoeol();
	while (i < max-polish_chars+1) { /* getting the right position in str */
		if (i == pos) {
			ret_val = j;
			mvwaddch(stdscr, rows-2, i, ' ');
			++i;
		}
		if (str[j] < 0) {
			bufpch[0] = str[j];
			bufpch[1] = str[j+1];
			mvwaddstr(stdscr, rows-2, i, bufpch);
			j+=2;
		}
		else {
			mvwaddch(stdscr, rows-2, i, str[j]);
			++j;
		}
		++i;
	}
	wrefresh(stdscr);
	for (i = max+flag; i > ret_val+flag; --i)
		str[i] = str[i-1-flag];
	return ret_val;
}

void add_line(char * str) {
	int i = (lnum < MAX_LINES) ? lnum : lnum % 65000;
	attr_t attrs;
	short pair;
	if (!lines) {
		if (!(lines = malloc(LINES_MEM_SEG * sizeof(Lines)))) {
			perror("Couldn't allocate memory for chat buffer.");
			end();
		}
	}
	if (lnum % LINES_MEM_SEG == 0 && lnum < MAX_LINES) {
		Lines * tmp = lines;
		if (!(tmp = realloc(lines, (lnum + LINES_MEM_SEG) * sizeof(Lines)))) {
			perror("Couldn't expand memory for chat buffer.");
			int j;
			for (j = 0; j < lnum; ++j) {
				free(lines[j].str);
			}
			free(lines);
			end();
		}
		lines = tmp;
	}

	if (!(lines[i].str = malloc((strlen(str) + 1) * sizeof(char)))) {
		int j;
		perror("Couldn't allocate memory for saving the line.");
		for (j = 0; j < lnum; ++j) {
			free(lines[j].str);
		}
		free(lines);
		end();
	}
	/* Everything has gone fine */
	strcpy(lines[i].str, str);
	wattr_get(chatbox, &attrs, &pair, NULL);
	lines[i].bold_flag = (attrs & A_BOLD) ? 1 : 0;
	++lnum;
}

void delete_lines() {
	if (lines) {
		unsigned short j;
		for (j = 0; j < lnum; ++j) {
			free(lines[j].str);	
		}
		free(lines);
	}
}

void display_lines() {
	short j = 0;
	unsigned short limit;
	short chatbox_rows = (posy + span_val > rows-CHATBOX_BOTTOM_SPAN-2) ? rows-CHATBOX_BOTTOM_SPAN-2 :
	rows-CHATBOX_BOTTOM_SPAN-2-span_val;
	int i = (lnum > chatbox_rows) ? lnum - chatbox_rows : 0;
	limit = (i > scroll_val) ? lnum-scroll_val : lnum-i;
	i = (i > scroll_val) ? i-scroll_val : 0;	
	while (i < limit) {
		if (lines[i].bold_flag)
			wattron(chatbox, A_BOLD);
		mvwaddstr(chatbox, j++, 1, lines[i].str); 
		if (lines[i].bold_flag)
			wattroff(chatbox, A_BOLD);
		++i;
	}
	posy = (lnum > chatbox_rows) ? chatbox_rows : lnum;
	wrefresh(chatbox);
}
