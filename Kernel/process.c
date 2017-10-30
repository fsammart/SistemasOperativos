#include "process.h"
static int pid=0;

StackFrame * fillStackFrame(void * entryPoint, StackFrame * userStack){
	StackFrame * frame = userStack - sizeof(StackFrame) -50;
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
	StackFrame * userStack=(StackFrame *)( ( malloc(100000)) + 100000 - MAX_PIPES*PIPE_LENGTH -1 -10*sizeof(Pipe) -1 -5*sizeof(int) -1);
	StackFrame * kernelStack= (StackFrame *) malloc(1000) + 1000;
	StackFrame * stack= fillStackFrame(entryPoint, userStack);
	p->userStack=stack;
	p->entryPoint=entryPoint;
	p->kernelStack=kernelStack;
	p->pid=pid++;
	p->state=READY;
	p->pipes = userStack + 1;
	p->pipesStruct=p->pipes +1;
	p->pipePids  = p->pipesStruct +1;
	p->description=description;
	return p;
}