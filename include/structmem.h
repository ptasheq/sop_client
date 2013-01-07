#ifndef STRUCTMEM_H
#define STRUCTMEM_H

#include "libs.h"
#include "protocol.h"

#define free_mem(ptr) {free(ptr); ptr = NULL;}

void * allocate_mem(const int, ...);

#endif
