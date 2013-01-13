#ifndef COMMUNICATION_H
#define COMMUNICATION_H	

#include "libs.h"
#include "protocol.h"

#define is_inroom() inroom

extern Msg_room * room_data;
extern Msg_chat_message chatmsg_data;
extern Msg_request_response * request_response_data;
extern Msg_request * request_data;
extern int inroom;
extern char roomname[ROOM_NAME_MAX_LENGTH];

void join(char *);
void leave();
void send_chatmsg(char *);
void send_priv(char *);
int get_username(char *);
void get_time(char *);
void display_request_result();
int wait_until_received(const int);
typedef void (*sa_handler)(int);
void set_signal(int, sa_handler);
#endif
