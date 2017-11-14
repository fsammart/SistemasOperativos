#include "stdio.h"
#include "dummy.h"

int consumerID[MAX_CONSUMER+1];
int producerID[MAX_PRODUCER+1];

int producers = 1;
int consumers = 1;

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

int main(void) {
	//Mutexes buffer access
	itemMutex = semCreate("itemMutex", 1 );

	//Counts full buffer slots
	fullCount = semCreate("fullCount", 0);

	//Counts empty buffer slots
	emptyCount = semCreate("emptyCount", bufferSize);

	//Create processes
	consumerID[0] = 0;
	producerID[0] = 0;
	createProcess(producer, "PRODUCER", &producerID[producers-1]);
	createProcess(consumer, "CONSUMER", &consumerID[consumers-1]);

 	control();
	//Control thread speed
	return 0;

}

int a = 10;
int i = 0;
void * producer(void * ctx) {

	int itemP;
	int fullP;
	int emptyP;

	itemP = semOpen("itemMutex");
	fullP = semOpen("fullCount");

	emptyP = semOpen("emptyCount");

	while (1) {

		sleep(prodSleepTime);
		int item = i++;
		int a[2];
		a[0] = *(int*)ctx;
		a[1] = item;

		printFF("Producer %d producing: %d\n", NULL, a);
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

		int a[2];
		a[0] = *(int*)ctx;
		a[1] = item;

		printFF("Consumer %d consuming: %d\n", NULL, a);

	}
}

void control() {
	int end = 0;

	while(!end) {
		int c = getchar();
		switch(c) {
			case 'c':
				if(consumers + 1 <= MAX_CONSUMER)
				{
					consumerID[consumers] = consumers;
					consumers++;
					createProcess(consumer, "CONSUMER", &consumerID[consumers-1]);
				}
				break;

			case 'p':
				if(producers + 1 <= MAX_PRODUCER)
				{
					producerID[producers] = producers;
					producers++;
					createProcess(producer, "PRODUCER", &producerID[producers-1]);
				}
		}
	}

}
