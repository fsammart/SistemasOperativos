#include <stdint.h>
#include <lib.h>
#include "naiveConsole.h"
#include "threads.h"
#include "stdio.h"
#include "scheduler.h"
#include "malloc.h"
#include "semaphores.h"

void insertItem(char c);
char removeItem();
void * producer(void * ctx);
void * consumer(void * ctx);
void control();



int itemMutex;
int emptyCount;
int fullCount;

int prodSleepTime = 1;
int consSleepTime = 100;

const int bufferSize = 50;

int r = 0;
int w = 0;

int bufferLength = 0;

char buffer[50];

void insertItem(char c) {
	buffer[w] = c;
	w = (w + 1) % bufferSize;
}

char removeItem() {
	char result = buffer[r];
	r = (r + 1) % bufferSize;
	return result;
}

int producerConsumer(void) {
	int i = 0;


	//Mutexes buffer access
	itemMutex = semCreate("itemMutex", 1 );
  //
	// if (itemMutex == SEM_FAILED) {
	// 	printf("Failed to create itemMutex\n");
	// 	perror("Error: ");
	// }

	//Counts full buffer slots
	fullCount = semCreate("fullCount", 0);

	// if (fullCount == SEM_FAILED) {
	// 	printf("Failed to create fullCount\n");
	// 	perror("Error: ");
	// }

	//Counts empty buffer slots
	emptyCount = semCreate("emptyCount", bufferSize);

	// if (emptyCount == SEM_FAILED){
	// 	printf("Failed to create emptyCount\n");
	// 	perror("Error: ");
	// }

	//Semaphore initialization

	ncPrint("Press enter to start\n");


	//Create processes
	createProcess(producer, "PRODUCER");

	createProcess(consumer, "CONSUMER");

 	 while(1);
	//Control thread speed

}

int a = 10;
int i = 0;
void * producer(void * ctx) {

	int item;

	int itemP;
	int fullP;
	int emptyP;

	itemP = semOpen("itemMutex");

	fullP = semOpen("fullCount");

	emptyP = semOpen("emptyCount");

	while (1) {
		sleep(prodSleepTime);

		int item = i++;
    	int a[1];
    	a[0] = item;
    	
		ncPrint("produce: ");
		ncPrintDec(item);
			ncPrint("\n");

		//Decrement the count of empty slots in the buffer (semaphore goes down)
		//Locks when the remaining empty slots are zero
		wait(emptyP);
		wait(itemP);

		insertItem(item);
		signal(itemP);

		//Increment the count of full slots in the buffer (semaphore goes up)
		signal(fullP);
	}
}

void * consumer(void * ctx) {
	int item;


	int itemC;
	int fullC;
	int emptyC;

	itemC = semOpen("itemMutex");

	fullC = semOpen("fullCount");

	emptyC = semOpen("emptyCount");

	while (1) {
		sleep(consSleepTime);

		//Decrement the count of full slots in the buffer (semaphore goes down)
		//Locks when there is no more information in the buffer
		wait(fullC);
		wait(itemC);

		item = removeItem();
		signal(itemC);

		//Increment the count of empty slots in the buffer (semaphore goes up)
		signal(emptyC);

    int a[1];
    a[0] = item;
	ncPrint("consume: ");
	ncPrintDec(item);
	ncPrint("\n");
	}
}