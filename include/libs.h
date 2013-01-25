#ifndef LIBS_H

#define LIBS_H

#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define STR_BUF_SIZE 256
#define USER_NAME_MAX_LENGTH 10
#define CHATBOX_BOTTOM_SPAN 4
#define MAX_FAILS 5
#define WAIT_TIME 100 
#define FAIL -1

extern WINDOW * chatbox; /* basic output for majority of modules*/
extern WINDOW * chatbox_border; /* window that is a border for chatbox */
extern WINDOW * msgbox; /* basic input for majority of modules */

extern int cols, rows; /* width and height of curses window */

void msleep(unsigned int); /* suspends thread for given miliseconds */

#endif
