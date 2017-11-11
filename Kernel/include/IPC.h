//IPC.h
#ifndef IPC_H

#define IPC_H

#define PIPE_LENGTH 10
#define MAX_PIPES 50
#define MAX_PROCESS_PIPES 3
#define MAX_LISTENERS 2

#define NO_SPACE_LEFT -1
#define PIPE_NOT_FOUND -2

#include "structs.h"
#include "lib.h"
#include "constants.h"

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
int getIndexForPipeStructure(Process * pro);
void freeSpace(char * name , Process * pro);
void closePipe(Pipe * pipe);
int getPipeNameIndex(char * name);

#endif
