#ifndef PROCESS
#define PROCESS_H

#include "lib.h"
#include "IPC.h"
#include "structs.h"
#include "stdio.h"
#include "naiveConsole.h"
#include "constants.h"

#define PIPE_LENGTH 10

Process * getProcess(void * entryPoint, char * description, void * args);
StackFrame * fillStackFrame(void * entryPoint, StackFrame * userStack, void * args);
void callProcess( void * entryPoint, void * entryPoint2, void * args);
int freeProcessPages(int pid);
void initializeThreadArray(Thread * t[], int dim);
Process *  getProcessById(int pid);
void initiatePipesForProcess(int * occupiedPosition);
void initializePipeFields(Process * p);

#endif
