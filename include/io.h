#ifndef IO_H
#define IO_H

#include "libs.h"
#include <wchar.h>
#include <locale.h>

#define INT_AS_STR_LENGTH 10 
#define ASCII_TO_INT 48

#define wants_exit(str) strcmp(str, "[exit]") == 0 ? 1 : 0
#define wants_send_chat_message(str) str[0] == '[' ? 0 : 1
#define wants_login(str) strcmp(str, "[login]") == 0 ? 1 : 0
#define wants_logout(str) strcmp(str, "[logout]") == 0 ? 1 : 0
#define wants_help(str) strcmp(str, "[help]") == 0 ? 1 : 0
#define wants_join_room(str) strncmp(str, "[join]", 6) == 0 ? 1 : 0
#define wants_leave_room(str) strcmp(str, "[leave]") == 0 ? 1 : 0
#define wants_users_list(str) strcmp(str, "[users]") == 0 ? 1 : 0
#define wants_rooms_list(str) strcmp(str, "[rooms]") == 0 ? 1 : 0
#define wants_room_users_list(str) strcmp(str, "[roomusers]") == 0 ? 1 : 0

short readstr(char *, int);
void writestr(char *);
void readint(int *);
int power(int, int);
int insert_wchar(wint_t, int (*)[2]);
wint_t decode_wchar(int (*)[2]);

#endif
