#include "io.h"


short readstr(char * str, int n) { /* Reads always one character less, so prevents overflow */
	wint_t ch;
	int i = 1, j = rows-1, k=0;
	cchar_t wbuf = {0};
	int wch[2];
	while (mvwget_wch(stdscr, rows-1, i, &ch) != FAIL) {
		if (k < STR_BUF_SIZE-1) {
			if ((ch > 31 && ch < 127)) {
				addch((char)ch);
				str[k] = (char) ch;
				++i;
				++k;
			}
			else if (ch == 127 && i > 1) {
				--k;
				--i;
				if (str[k] < -1) 
					--k;
				mvdelch(rows-1, i);
			}
			else if (insert_wchar(ch, &wch) != FAIL && k < STR_BUF_SIZE-2) {
				wbuf.chars[0] = ch;
				add_wch(&wbuf);
				str[k] = wch[0];
				str[k+1] = wch[1];
				++i;
				k+=2;
			}
			else if (ch == 10) {
				if (wants_exit(str)) {
					end();
				}
				str[k] = '\0';
				writestr(str);
				while (i > 1) {
					--i;
					mvdelch(rows-1, i);
				}
				k = 0;
			}
		}
	 /* 410 127 */
	}
		
	if (mvwgetnstr(stdscr, rows-1, 0, str, n-1) == OK) {
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
	int ch;
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

int insert_wchar(wint_t ch, int (*letter)[2]) {
	switch(ch) {
		case 260: case 261:
		case 262: case 263:
		case 280: case 281:
			(*letter)[0] = -60;
			(*letter)[1] = ch - 384;
			break;
		case 321: case 322: 
		case 323: case 324:
		case 346: case 347: 
		case 377: case 378:
		case 379: case 380:
			(*letter)[0] = -59;
			(*letter)[1] = ch - 448;
			break;
		case 211: case 243:	
			(*letter)[0] = -61;
			(*letter)[1] = ch - 320;
			break;
		default:
			(*letter)[0] = FAIL;
	}
	return (*letter)[0];
} 

wint_t decode_wchar(int (*letter)[2]) {
	switch((*letter)[0]) {
		case -59:
			return (*letter)[0] + 448;
		case -60:
			return (*letter)[0] + 384;
		case -61:
			return (*letter)[0] + 320;
		default:
			return FAIL;
	}
}
