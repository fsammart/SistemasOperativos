//test.IPC

#include <stdint.h>
#include <lib.h>
#include "naiveConsole.h"
#include "threads.h"
#include "stdio.h"
#include "scheduler.h"
#include "malloc.h"

void thread();

void thread(void * args)
{
	char * dato = (char*)args;
	for(int i = 0; i<10; i++)
	{
		putchar(dato[i]);
	}
	putchar('d');

}
void processA()
{
	char buffer[10];
	int i;
	char c = 'a';
	for(i = 0; i<10; i++)
	{
		buffer[i] = c++;
	}
	sys_call(12,(qword)thread, (qword)buffer);
	while(1);

}

void processB()
{
	outputb(0x20, 0x20);
	// ncPrint("LLegueB");
	// char * h = (char*)sys_call(13,4048,0);
	// ncPrintHex(h);
	// for (int i = 0; i < 200; ++i)
	// {
	// 	h[i] = 'y';
	// }
	// ncPrint(h);
	// ncPrint("B------");
	// ncPrintDec(h);
	// h = (char*)sys_call(13,4,0);
	// for (int i = 0; i < 5; ++i)
	// {
	// 	h[i] = 'j';
	// }
	// ncPrint("------");
	// ncPrint(h);
	// ncPrint("PasoB");
	// ncPrint("------");
	while(1);

}
