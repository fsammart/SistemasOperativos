//pipes.h
#ifndef PIPES

#define PIPES 1 

#define PIPE_LENGTH 10
#define MAX_PIPES 50
#define NULL (void*)0

#include "structs.h"
#include "lib.h"

Pipe *  createPipe(int pid , char * name);
Pipe * createPipeStruct(Process * pro,char * name);
void assignPipe(Pipe * pipe, Process * p);
int write(Pipe * p , char * message);
int read ( Pipe * p , char * result, int bytes);
char * next( char * aux, Pipe * pipe);
Pipe *  openPipe(char * name);
Process *  getProcessById(int pid);
void unlockProcesses(char * name);
void changeProcessState(int pid, processState state);
int getCurrentPid();

#endif