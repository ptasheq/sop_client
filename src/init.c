#include "libs.h"

void init() {
	gfx_init();
	login();
}

void end() {
	logout();
	gfx_free();
}
