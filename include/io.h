#ifndef IO_H
#define IO_H

#include "libs.h"

#define INT_AS_STR_LENGTH 9 
#define ASCII_TO_INT 48

#define wants_exit(str) strcmp(str, "[exit]") == 0 ? 1 : 0

short readstr(char *, int);
void writestr(char *);
void readint(int *);
int power(int, int);

#endif
