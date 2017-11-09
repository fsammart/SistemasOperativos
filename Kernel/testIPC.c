//test.IPC

#include <stdint.h>
#include <lib.h>
#include "naiveConsole.h"
#include "threads.h"
typedef struct Pipe * Pipe;
void thread();

void thread()
{
	putchar('d');
}
void processA()
{
	char buffer[10];
	int i;
	int r;
	Pipe  pipe2;
	sleep(50); //wait until pipe is created
	pipe2 = openPipe("prueba1");
	createThread(thread);
	while(1){
		sleep(50);
		r = read(pipe2, buffer, -1);
		/*putchar('=');
		for(i=0; i < r; i++){
			putchar(buffer[i]);
		}
		putchar('=');*/
	}	
}

void processB()
{
	Pipe  pipe = createPipe(0 , "prueba1");
	while(1){
		sleep(100);
		write(pipe, "Emocion");
	}
}