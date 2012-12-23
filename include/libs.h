#ifndef LIBS_H

#define LIBS_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define STR_BUF_SIZE 256
#define CHATBOX_BOTTOM_SPAN 4

extern WINDOW * chatbox; // basic output for majority of modules
extern WINDOW * chatbox_border; // window that is a border for chatbox

extern int cols, rows;


#endif
