#include "gui.h"
#include <ncursesw/ncurses.h>
#include <locale.h>

int cols, rows;
WINDOW * chatbox, * chatbox_border;

WINDOW * create_newwin(int starty, int startx, int endy, int endx, int border_flag) {

    WINDOW * tmp;
    if(!(tmp = newwin(endy, endx, starty, startx))) {
		endwin();
		perror("Couldn't create window\n");
		exit(EXIT_FAILURE);	
	}
    wbkgd(tmp, COLOR_PAIR(1));
    scrollok(tmp, TRUE);
    if (border_flag) {
        box(tmp, 0, 0);
    }
    wrefresh(tmp);
    return tmp;
}

void init_colors() {
    if (!has_colors()) {
        endwin();
        perror("Your terminal does not support color\n");
        exit(EXIT_FAILURE);
    }
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    wbkgd(stdscr, COLOR_PAIR(1));
}

void gfx_init() {
	setlocale(LC_CTYPE, "pl_PL.utf8");
	initscr();
	init_colors();
	getmaxyx(stdscr, rows, cols);
	if (rows < 0 || cols < 0) {
		endwin();
		perror("Unable to gain info about console window size\n");
		exit(EXIT_FAILURE);
	}
	refresh();
	chatbox_border = create_newwin(1, 1, rows-CHATBOX_BOTTOM_SPAN, cols-CHATBOX_BOTTOM_SPAN, DRAW_BORDER); 
	chatbox = create_newwin(2, 2, rows-CHATBOX_BOTTOM_SPAN-2, cols-CHATBOX_BOTTOM_SPAN-2, DRAW_NO_BORDER);
}

void gfx_free() {
	delwin(chatbox);
	delwin(chatbox_border);
	endwin();
}



