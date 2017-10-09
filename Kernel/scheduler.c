#include "scheduler.h"
#include <naiveConsole.h>

static ProcessSlot * current=NULL;

static int cardinal_processes=0;

ProcessSlot * newProcessSlot(Process * process){
	ProcessSlot * newProcessSlot = (ProcessSlot *) malloc(1000);
	newProcessSlot->process=process;
	return newProcessSlot;
}
int i=0;
void  createProcess(void * entryPoint, char * description){
	Process * p=getProcess(entryPoint);
	p->pid=pid++;
	p->state=READY;
	p->description=description;
	addProcess(p);
	cardinal_processes++;

}
int getCurrentPid() {
	return current==NULL?-1:current->process->pid;
}

void changeProcessState(int pid, processState state) {

	int i = 0;
    ProcessSlot * slot = current;
	for (; i < cardinal_processes; i++) {
		if (slot->process->pid == pid) {
			slot->process->state = state;
			return;
		}
		slot = slot->next;
	}
	return ; //pid doesnt exist
}

Process * getProcess(void * entryPoint){
	Process * p =(Process *) malloc(1000);
	StackFrame * userStack= (StackFrame *) malloc(100000);
	StackFrame * kernelStack= (StackFrame *) malloc(1000);
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
		current->next->next=next;

	}
}

void * next_process(int current_rsp) {
	if (current == NULL) {
		return current_rsp;
	}
	current->process->userStack = current_rsp;

	schedule();
    int ans=current->process->userStack;
    return ans;
}
//void removeProcess(Process * process); TODO
void schedule(){
	if(current->process->state == RUNNING)
		current->process->state = READY;

	current = current->next;
	while (current->process->state != READY) {
				current = current->next;
		}

	current->process->state = RUNNING;
}
/* returns kernel stack*/
StackFrame * switchUserToKernel(void * esp){

	Process * process = current->process;
	process->userStack=esp;
	return process->kernelStack;
}
/* returns next process from scheduler*/
StackFrame * switchKernelToUser(){
	schedule();
	return current->process->userStack;
}

void * getCurrentEntryPoint(){
	return current->process->entryPoint;
}

Process * *  getCurrentProcesses(int * a){
	Process ** processes= (Process * *) malloc(cardinal_processes*sizeof(Process *));
	ProcessSlot  * aux= current;
	int i=0;
	processes[i]=aux->process;
	aux=aux->next;
	while(i<cardinal_processes){
		i++;
		processes[i]=aux->process;
		aux=aux->next;

	}
	*a=cardinal_processes;
	return processes;
}

void callProcess( void * entryPoint) {
	((int (*)(void))(entryPoint))();

	//sys_leave(0, 0, 0, 0, 0);
}

StackFrame * fillStackFrame(void * entryPoint, StackFrame * userStack){
	StackFrame * frame = userStack - 1;
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
	frame->rip =	entryPoint;
	frame->cs =		0x008;
	frame->eflags = 0x202;
	frame->rsp =	(uint64_t)&(frame->base);
	frame->ss = 	0x000;
	frame->base =	0x000;

	return frame;
}
