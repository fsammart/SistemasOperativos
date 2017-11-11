//test.IPC

#include <stdint.h>
#include <lib.h>
#include "naiveConsole.h"
#include "threads.h"
#include "stdio.h"

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
	thread_create(thread, buffer);
	putchar('a');
	while(1);

}

void processB()
{
	outputb(0x20, 0x20);
	while(1);

}
