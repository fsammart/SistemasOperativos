//structs.h
#ifndef STRUCTS
#define STRUCTS 0 

#define MAX_PAGES_PER_PROCESS 10

#include "lib.h"

typedef enum processState_t {RUNNING, READY, BLOCKED, DEAD, SLEEPING} processState;

typedef struct Pipe_s {
	char * name;
	int position;
	int end;
	int creator;
	int * pipePids;
	int * blocked;
	int cardinalBlocked;
	int connected;
	char * pipe;
	int readFlag;
}Pipe;

typedef struct StackFrame_s{
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

typedef struct Thread_s{
	StackFrame * userStack;
	StackFrame * kernelStack;
	void * entryPoint;
}Thread;


typedef struct Process_s{

	int pid;
	processState state;
	char * description;

	int activeThread;
	int numberOfThreads;

	Thread * thread[3];

	int * pipePids;
	int * blocked;
	Pipe *  pipesStruct;
	char *  pipes;

	void * pages[MAX_PAGES_PER_PROCESS];

}Process;


#endif