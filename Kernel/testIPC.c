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
	sys_call(12,thread, buffer);
	ncPrint("LLegue");
	char * h = kmalloc(5);
	ncPrint("#");
	ncPrintDec(h);
	ncPrint("#");
	ncPrint("------");
	char * g = kmalloc(8);
	g[0] = 'H';
	g[1] = 'o';
	g[2] = 'l';
	g[3] = 'a';
	g[4] = 0;
	ncPrint("#");
	ncPrintDec(g);
	ncPrint("#");
	ncPrint("------");
	h = kmalloc(15);
	ncPrint("#");
	ncPrintDec(h);
	ncPrint("#");
	ncPrint("Paso");
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
