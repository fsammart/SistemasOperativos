#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "stdio.h"
#include "lib.h"
#include "interrupts.h"
#include "systemCalls.h"
#include <naiveConsole.h>
#include "process.h"
#include "structs.h"
#include "constants.h"

typedef void (*EntryPointHandler) (void*);

typedef struct ProcessSlotS {
	struct ProcessSlotS * next;
	Process * process;
}ProcessSlot;

ProcessSlot * newProcessSlot(Process * process);
void createProcess(void * entryPoint, char * description);
int getCurrentPid();
void changeProcessState(int pid, processState state);
void addProcess(Process * process);
void * next_process(void * current_rsp);
void schedule();
StackFrame * switchUserToKernel(void * esp);
StackFrame * switchKernelToUser();
void * getCurrentEntryPoint();
Process * *  getCurrentProcesses(int * a);
Process * getCurrentProcess();
int eqProcess(Process * a, Process * b);
char * getStateFromNumber(int state);
void printProcesses();
void removeProcess(int pid);
void callProcess( void * entryPoint, void * entryPoint2, void * args) ;
void beginScheduler();
StackFrame * getCurrentUserStack();
void restartSHELL();

#endif
