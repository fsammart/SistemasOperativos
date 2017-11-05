#include "process.h"
static int pid=0;

StackFrame * fillStackFrame(void * entryPoint, StackFrame * userStack){
	StackFrame * frame =userStack - 2;
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
	frame->rsi =	(uint64_t )entryPoint;
	frame->rdi =	(uint64_t)entryPoint; //entryPoint;
	frame->rbp =	0x00D;
	frame->rdx =	0x00E;
	frame->rcx =	0x00F;
	frame->rbx =	0x010;
	frame->rax =	0x011;
	frame->rip =	(uint64_t) &callProcess;
	frame->cs =		0x008;
	frame->eflags = 0x202;
	frame->base =	userStack - 50 -1;
	frame->rsp =	(uint64_t)&(frame->base);
	frame->ss = 	0x000;

	return frame;
}

Process * getProcess(void * entryPoint , char * description){
	Process * p =(Process *) malloc(1000);
	void  * page1= allocPage(2);
	void * page2 = allocPage(2);
	StackFrame * userStack=((char *)page1)+ 2*1024*4 - MAX_PROCESS_PIPES*PIPE_LENGTH -1 -MAX_PROCESS_PIPES*sizeof(Pipe) -1 -MAX_PROCESS_PIPES*MAX_LISTENERS*sizeof(int) -1 - MAX_PROCESS_PIPES*sizeof(int) -MAX_PROCESS_PIPES*sizeof(int)-10;
	StackFrame * kernelStack= ((char *)page2) + 2*1024*4;
	StackFrame * stack= fillStackFrame(entryPoint, userStack); 
	p->userStack = stack;
	p->entryPoint = entryPoint;
	p->kernelStack = kernelStack;
	p->pid = pid++;
	p->state = READY;
	p->pipes = userStack + 1; //array de char  *
	p->pipesOpened = 0;
	p->pipesStruct = (char * ) p->pipes + MAX_PROCESS_PIPES*sizeof(Pipe) + 1; //array de Pipesstruct
	p->pipePids  = (char * )p->pipesStruct + MAX_PROCESS_PIPES*sizeof(int) +1; //array de pids
	p->blocked = (char *)p->pipePids + MAX_PROCESS_PIPES*sizeof(int) +  1; //array de pids
	p->occupiedPosition = (char *) p->blocked + MAX_PROCESS_PIPES*MAX_LISTENERS*sizeof(int) +  1; //array de int
	p->description = description;

	initiatePipesForProcess(p->occupiedPosition);

	return p;
}

void initiatePipesForProcess(int * occupiedPosition)
{
	int i = 0;
	for(; i < MAX_PROCESS_PIPES ; i++){
		occupiedPosition[i]=0;
	}
}

int freeProcessPages(int pid)
{
	Process  * p = getProcessById(pid);
	ncPrintDec(p->pid);
	deallocPage(p->userStack);
	deallocPage(p->kernelStack);
}