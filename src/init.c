#include "init.h"
#include "listener.h"
#include <signal.h>
#include <sys/types.h>

int ch_pid;

void init() {
	listener_init();
	gfx_init();
	login();
}

void end() {
	logout(0);
	listener_end();
	gfx_free();
	exit(EXIT_SUCCESS);
}

void msleep(unsigned int msec) {
	usleep(1000*msec);
}
