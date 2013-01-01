#ifndef LIBS_H

#define LIBS_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define STR_BUF_SIZE 256
#define CHATBOX_BOTTOM_SPAN 4
#define MAX_FAILS 5
#define FAIL -1

extern WINDOW * chatbox; /* basic output for majority of modules*/
extern WINDOW * chatbox_border; /* window that is a border for chatbox */

extern int cols, rows;

void msleep(unsigned int); /* suspends thread for given miliseconds */

#endif
