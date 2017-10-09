#ifndef SCHEDULER

#include "stdio.h"
#include "lib.h"
#include "interrupts.h"
#define SCHEDULER 0

#define NULL ((void *)0)

static int pid=0;

typedef enum processState_t {RUNNING, READY, BLOCKED, DEAD, SLEEPING} processState;

typedef struct StackFrameS {
	//Registers restore context
	uint64_t gs;
	uint64_t fs;
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rbp;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;

	//iretq hook
	uint64_t rip;
	uint64_t cs;
	uint64_t eflags;
	uint64_t rsp;
	uint64_t ss;
	uint64_t base;
}StackFrame;

typedef struct ProcessS {
	StackFrame * userStack;
	StackFrame * kernelStack;
	void * entryPoint;
	int pid;
	processState state;
	char * description;

}Process;

typedef struct ProcessSlotS {
	struct ProcessSlotS * next;
	Process * process;
}ProcessSlot;



Process * getProcess(void * entryPoint);
ProcessSlot * newProcessSlot(Process * process);


void addProcess(Process * process);
//void removeProcess(Process * process);
void schedule();
/* returns kernel stack*/
StackFrame * switchUserToKernel(void * esp);
void createProcess(void * entryPoint, char * description);
/* returns next process from scheduler*/
StackFrame * switchKernelToUser();
StackFrame * fillStackFrame(void * entryPoint, StackFrame * userStack);
void changeProcessState(int pid, processState state);
void * next_process(int current_rsp);
void callProcess( void * entryPoint);
Process * *  getCurrentProcesses(int * a);
#endif