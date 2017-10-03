#include "scheduler.h"

ProcessSlot * current=NULL;
void * kernel_stack=NULL;
int i =0;

ProcessSlot * newProcessSlot(Process * process){
	ProcessSlot * newProcessSlot = malloc(sizeof(ProcessSlot));
	newProcessSlot->process=process;
	return newProcessSlot;
}

void  createProcess(void * entryPoint){
	Process * p=getProcess(entryPoint);
	addProcess(p);
}

Process * getProcess(void * entryPoint){
	Process * p = malloc(sizeof(Process));
	StackFrame * userStack= malloc(100000);
	StackFrame * kernelStack= malloc(100000);
	StackFrame * stack= fillStackFrame(entryPoint, userStack);
	p->userStack=stack;
	p->entryPoint=entryPoint;
	p->kernelStack=kernelStack;
	return p;
}

void addProcess(Process * process){
	ProcessSlot * newProcess = newProcessSlot(process);
	if(current==NULL){
		current=newProcess;
		current->next=current;
	}else{
		ProcessSlot * next = current->next;
		current->next=newProcess;
		newProcess->next=next;
	}
}
//void removeProcess(Process * process); TODO
void schedule(){
	current=current->next;
}
/* returns kernel stack*/
void * switchUserToKernel(void * esp){
	if(current!=NULL) {
	Process * process = current->process;
	process->userStack=esp;
	return process->kernelStack;
	}
	return esp;
}
/* returns next process from scheduler*/
void * switchKernelToUser(void * esp){

	if(current!=NULL){
	schedule();

	return current->process->userStack;
	}else {return esp;}
}


void * fillStackFrame(void * entryPoint, void * userStack){
	StackFrame * frame = (StackFrame*)userStack - 1;
	frame->gs =		0x001;
	frame->fs =		0x002;
	frame->r15 =	0x003;
	frame->r14 =	0x004;
	frame->r13 =	0x005;
	frame->r12 =	0x006;
	frame->r11 =	0x007;
	frame->r10 =	0x008;
	frame->r9 =		0x009;
	frame->r8 =		0x00A;
	frame->rsi =	0x00B;
	frame->rdi =	0x00C;
	frame->rbp =	0x00D;
	frame->rdx =	0x00E;
	frame->rcx =	0x00F;
	frame->rbx =	0x010;
	frame->rax =	0x011;
	frame->rip =	(uint64_t)entryPoint;
	frame->cs =		0x008;
	frame->eflags = 0x202;
	frame->rsp =	(uint64_t)&(frame->base);
	frame->ss = 	0x000;
	frame->base =	0x000;

	return frame;
}
