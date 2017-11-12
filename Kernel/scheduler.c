#include "scheduler.h"
#include "threads.h"
#include "buddyAllocator.h"

static int cardinal_processes=0;
static ProcessSlot * current=NULL;

ProcessSlot * newProcessSlot(Process * process){
	ProcessSlot * newProcessSlot = (ProcessSlot *) allocPage(NUMBER_OF_PAGES_TO_ALLOC(sizeof(ProcessSlot)));
	newProcessSlot->process=process;
	return newProcessSlot;
}

void createProcess(void * entryPoint, char * description){
	Process * p = getProcess(entryPoint, description, NULL);
	addProcess(p);
	cardinal_processes++;
}

Process * getProcessById(int pid){
	int i;
	ProcessSlot * slot = current;

	for(i=0; i < cardinal_processes ; i++){
		if(slot[i].process->pid == pid){
			return slot[i].process;
		}
	}

	return NULL;
}

int getCurrentPid() {
	return current==NULL?-1:current->process->pid;
}

Process * getCurrentProcess(){
	return current->process;
}

void changeProcessState(int pid, processState state) {
	int i;
  ProcessSlot * slot = current;

	for (i = 0; i < cardinal_processes; i++){
		if (slot->process->pid == pid){
			slot->process->state = state;
			return;
		}
		slot = slot->next;
	}
	return; //pid doesnt exist
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

StackFrame * getCurrentUserStack(){
		return current->process->thread[current->process->activeThread]->userStack;
}

void * next_process(void * current_rsp){
	void * ans;
	if (current == NULL){
		return current_rsp;
	}

	current->process->thread[current->process->activeThread]->userStack = current_rsp;
	schedule();
  ans = current->process->thread[current->process->activeThread]->userStack;
  return ans;
}

void schedule(){
	if(current->process->state == RUNNING){
		current->process->state = READY;
	}

	current = current->next;
	while (current->process->state != READY) {
				current = current->next;
		}

	current->process->state = RUNNING;
}

int nextThread()
{
	int i = 1;
	Process * p = getCurrentProcess();
	while( p->thread[(p->activeThread + i )%(3)] == NULL){
		i++;
	}

	return (p->activeThread + i )%(3);
}
/* returns kernel stack*/
StackFrame * switchUserToKernel(void * esp){

	Process * process = current->process;

	process->thread[process->activeThread]->userStack=esp;
	return process->thread[process->activeThread]->kernelStack;
}
/* returns next process from scheduler*/
StackFrame * switchKernelToUser(){
	schedule();
	int index = nextThread();


	current->process->activeThread = index;
	return current->process->thread[index]->userStack;
}

void * getCurrentEntryPoint(){
	return current->process->thread[current->process->activeThread]->entryPoint;

}

Process * *  getCurrentProcesses(int * a){
	Process ** processes= (Process * *) allocPage(NUMBER_OF_PAGES_TO_ALLOC(cardinal_processes*sizeof(Process *)));
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

int eqProcess(Process * a, Process * b){
	return a->pid == b->pid;
}
char * getStateFromNumber(int state){
	char * s;
	switch(state){
			case 0: s="RUNNING";
					break;
			case 1:s="READY";
					break;
			case 2:s= "BLOCKED";
					break;
			case 3: s="DEAD";
					break;
			case 4: s="SLEEPING";
					break;
			default: s="other";
		}
		return s;

}

void printProcesses(){
	int c=0;
	int i;
	char * state;
	char * stack;
	Process ** s= getCurrentProcesses(&c);
	for(i=0; i<c; i++){
		print("pid: ");
		putNumber(s[i]->pid);
		print("-->description: ");
		print(s[i]->description);
		print("-->state: ");
		state=getStateFromNumber(s[i]->state);
		print(state);
		print("  UserStack:");
		stack = (char*)s[i]->thread[0]->userStack;
		ncPrintHex((uint64_t)stack);

		putchar('\n');
	}
}

void removeProcess(int pid) {
	if (current == NULL) {
		return;

	} else if(eqProcess(current->process, current->next->process) && current->process->pid==pid) {
		// process to remove is the current and only one process in list
		if(pid==0) return;
       freeProcessPages(pid);
       current = NULL;
       cardinal_processes--;
       return;
	}

	ProcessSlot * prevSlot = current;
	ProcessSlot * slotToRemove = current->next;

	while (slotToRemove->process->pid != pid) {
		prevSlot = slotToRemove;
		slotToRemove = slotToRemove->next;
	}

	prevSlot->next = slotToRemove->next;
	cardinal_processes--;

	if (eqProcess(slotToRemove->process, current->process)) {
		_yield();
	}
}

void callProcess(void * entryPoint, void * entryPoint2, void * args){

	((EntryPointHandler)entryPoint2)(args);

	terminateThread();

	_yield();


}

void beginScheduler() {
	((int (*)(void))(current->process->thread[0]->entryPoint))();
}
