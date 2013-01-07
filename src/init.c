#include "init.h"
#include "listener.h"
#include <signal.h>
#include <sys/types.h>

int ch_pids[2] = {0};

void init() {
	gfx_init();
	listener_init();
	login();
}

void end() {
	logout();
	listener_end();
	gfx_free();
	exit(EXIT_SUCCESS);
}

void msleep(unsigned int msec) {
	usleep(1000*msec);
}

void end_children() {
	int i = 0;
	while (i < 2) {
		if (ch_pids[i])
			kill(ch_pids[i], SIGALRM);
		++i;
	}
}
