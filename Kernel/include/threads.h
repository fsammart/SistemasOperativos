#ifndef THREADS_H
#define THREADS_H

#define MAX_THREADS 3
#include "structs.h"
#include "constants.h"

int createThread(void * entryPoint, void * args);
void terminateThread();
int findNextSpot(Thread * t[], int dim);
void freeThreadPages(Thread * t);

#endif
