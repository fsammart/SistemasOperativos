#ifndef PROCESS

#define PROCESS 0

#include "lib.h"

#include "pipes.h"

#include "structs.h"

#include "stdio.h"

#define PIPE_LENGTH 10

Process * getProcess(void * entryPoint, char * description);

StackFrame * fillStackFrame(void * entryPoint, StackFrame * userStack);

void callProcess( void * entryPoint, void * entryPoint2) ;

#endif