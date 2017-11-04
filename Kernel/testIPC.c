//test.IPC

#include <stdint.h>
#include <lib.h>
#include "naiveConsole.h"

typedef struct Pipe * Pipe;

void processA()
{
	ncPrint("PROCESS A");
	char buffer[10];
	int i;
	int r;
	Pipe  pipe2;
	sleep(50); //wait until pipe is created
	pipe2 = openPipe("prueba1");
	ncPrint("PIPE ABIERTO");
	while(1){
		sleep(50);
		ncPrint("leyendo");
		r = read(pipe2, buffer, -1);
		putchar('=');
		for(i=0; i < r; i++){
			putchar(buffer[i]);
		}
		putchar('=');
	}	
}

void processB()
{
	ncPrint("PROCESS B");
	Pipe  pipe = createPipe(0 , "prueba1");
	ncPrint("PIPE CREADO");
	while(1){
		sleep(100);
		write(pipe, "Emocion");
	}
}