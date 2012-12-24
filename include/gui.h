#ifndef GUI_H
#define GUI_H	

#include "libs.h"

const int DRAW_BORDER = 1;
const int DRAW_NO_BORDER = 0;

WINDOW * create_newwin(int, int, int, int, int); // creates new window within console screen
void destroy_win(WINDOW *); // erases window from screen and memory
void init_colors(); // enables the color module
void gfx_init(); // graphics initialization
void gfx_free(); // graphics uninitalization

#endif
