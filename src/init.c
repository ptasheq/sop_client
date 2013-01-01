#include "init.h"

void init() {
	gfx_init();
	login();
}

void end() {
	logout();
	gfx_free();
	exit(EXIT_SUCCESS);
}

void msleep(unsigned int msec) {
	usleep(1000*msec);
}
